#pragma ocne

#include "db_wrapper.hpp"

#include <rocksdb/db.h>
#include <rocksdb/options.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static const char* rocksdb_path = "rocksDb_events.db";


class RocksDBWrapper : public DBWrapper<rocksdb::DB> {
public:
    RocksDBWrapper() {
        rocksdb::Options options;
        options.create_if_missing = true;
        rocksdb::Status status = rocksdb::DB::Open(options, rocksdb_path, &db);
        if (!status.ok()) {
            std::cerr << "Unable to open/create database: " << rocksdb_path << std::endl;
            std::cerr << status.ToString() << std::endl;
            db = nullptr;
        }
    }

    ~RocksDBWrapper() override {
        delete db;
    }

    void createTable() override {
    }

    void insertEvent(int id, const std::string& event_data, const std::string& event_type) override {
        Event event{id, event_data, event_type, "pending"};
        std::string key = std::to_string(id);
        std::string value = serializeEvent(event);
        db->Put(rocksdb::WriteOptions(), key, value);
    }

    std::vector<Event> fetchPendingEvents(int limit) override {
        std::vector<Event> events;
        auto it = db->NewIterator(rocksdb::ReadOptions());
        for (it->SeekToFirst(); it->Valid() && events.size() < limit; it->Next()) {
            Event event = deserializeEvent(it->value().ToString());
            if (event.status == "pending") {
                events.push_back(event);
            }
        }
        delete it;
        return events;
    }

    std::vector<Event> fetchAndMarkPendingEvents(int limit) override {
        std::vector<Event> events;
        rocksdb::WriteBatch batch;
        auto it = db->NewIterator(rocksdb::ReadOptions());
        for (it->SeekToFirst(); it->Valid() && events.size() < limit; it->Next()) {
            Event event = deserializeEvent(it->value().ToString());
            if (event.status == "pending") {
                event.status = "processing";
                events.push_back(event);
                batch.Put(it->key(), serializeEvent(event));
            }
        }
        rocksdb::Status s = db->Write(rocksdb::WriteOptions(), &batch);
        delete it;
        return events;
    }

    void updateEventStatus(const std::vector<int>& event_ids) override {
        for (int id : event_ids) {
            std::string key = std::to_string(id);
            std::string value;
            db->Get(rocksdb::ReadOptions(), key, &value);
            Event event = deserializeEvent(value);
            event.status = "dispatched";
            db->Put(rocksdb::WriteOptions(), key, serializeEvent(event));
        }
    }

    int getPendingEventCount() override {
        int count = 0;
        auto it = db->NewIterator(rocksdb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next()) {
            Event event = deserializeEvent(it->value().ToString());
            if (event.status == "pending") {
                ++count;
            }
        }
        delete it;
        return count;
    }

private:
    rocksdb::DB* db = nullptr;

    std::string serializeEvent(const Event& event) {
        json j;
        j["id"] = event.id;
        j["event_data"] = event.event_data;
        j["event_type"] = event.event_type;
        j["status"] = event.status;
        return j.dump();
    }

    Event deserializeEvent(const std::string& event_str) {
        json j = json::parse(event_str);
        return {j["id"], j["event_data"], j["event_type"], j["status"]};
    }
};
