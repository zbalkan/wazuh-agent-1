/*
 * Wazuh Inventory
 * Copyright (C) 2015, Wazuh Inc.
 * January 12, 2021.
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public
 * License (version 2) as published by the FSF - Free Software
 * Foundation.
 */
#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include <map>

class InvNormalizer
{
    public:
        InvNormalizer(const std::string& configFile,
                      const std::string& target);
        ~InvNormalizer() = default;
        void Normalize(const std::string& type, nlohmann::json& data) const;
        void RemoveExcluded(const std::string& type, nlohmann::json& data) const;
    private:
        static std::map<std::string, nlohmann::json> GetTypeValues(const std::string& configFile,
                                                                   const std::string& target,
                                                                   const std::string& type);
        const std::map<std::string, nlohmann::json> m_typeExclusions;
        const std::map<std::string, nlohmann::json> m_typeDictionary;
};
