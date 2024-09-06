#include <gtest/gtest.h>

#include <command_store.hpp>

constexpr int FIVE = 5;
constexpr int NINE = 9;
constexpr int ELEVEN = 11;

class CommandStoreTest : public ::testing::Test
{
protected:
    std::unique_ptr<command_store::CommandStore> m_commandStore;

    void SetUp() override
    {
        m_commandStore = std::make_unique<command_store::CommandStore>();
    }

    void TearDown() override {}
};

TEST_F(CommandStoreTest, StoreCommandTest)
{
    m_commandStore->Clear();

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd1(
        FIVE, "Module1", "{CommandTextHERE}", "Parameter1", "Result1", command_store::Status::IN_PROGRESS);
    bool retVal = m_commandStore->StoreCommand(cmd1);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd2(
        NINE, "Module2", R"({"Some"="thing"})", "Parameter2", "Result2", command_store::Status::IN_PROGRESS);
    retVal = m_commandStore->StoreCommand(cmd2);
    ASSERT_EQ(retVal, true);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd3(
        FIVE, "Module3", "{CommandTextHERE}", "Parameter3", "Result3", command_store::Status::IN_PROGRESS);
    retVal = m_commandStore->StoreCommand(cmd3);
    ASSERT_EQ(retVal, false);

    ASSERT_EQ(m_commandStore->GetCount(), 2);
}

TEST_F(CommandStoreTest, UpdateCommandTest)
{
    m_commandStore->Clear();

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd1(
        FIVE, "Module1", "{CommandTextHERE}", "Parameter1", "Result1", command_store::Status::IN_PROGRESS);
    m_commandStore->StoreCommand(cmd1);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd2(
        NINE, "Module2", R"({"Some"="thing"})", "Parameter2", "Result2", command_store::Status::IN_PROGRESS);
    m_commandStore->StoreCommand(cmd2);

    ASSERT_EQ(m_commandStore->GetCount(), 2);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmdUpdate(NINE,
                                     "Updated Module",
                                     "Updated Command",
                                     "Updated Parameter",
                                     "Updated Result",
                                     command_store::Status::SUCCESS);

    bool retVal = m_commandStore->UpdateCommand(cmdUpdate);
    ASSERT_EQ(retVal, true);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    std::optional<command_store::Command> retValue = m_commandStore->GetCommand(9);
    if (retValue.has_value())
    {
        const command_store::Command& cmd = retValue.value();
        ASSERT_EQ(cmd.m_id, NINE);
        ASSERT_EQ(cmd.m_module, "Updated Module");
        ASSERT_EQ(cmd.m_command, "Updated Command");
        ASSERT_EQ(cmd.m_parameters, "Updated Parameter");
        ASSERT_EQ(cmd.m_result, "Updated Result");
        ASSERT_EQ(cmd.m_status, command_store::Status::SUCCESS);
    }
    else
    {
        FAIL();
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmdUpdate2(NINE, "", "", "", "Newly Updated Result", command_store::Status::UNKNOWN);

    retVal = m_commandStore->UpdateCommand(cmdUpdate2);
    ASSERT_EQ(retVal, true);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    retValue = m_commandStore->GetCommand(NINE);
    if (retValue.has_value())
    {
        const command_store::Command& cmd = retValue.value();
        ASSERT_EQ(cmd.m_id, NINE);
        ASSERT_EQ(cmd.m_module, "Updated Module");
        ASSERT_EQ(cmd.m_command, "Updated Command");
        ASSERT_EQ(cmd.m_parameters, "Updated Parameter");
        ASSERT_EQ(cmd.m_result, "Newly Updated Result");
        ASSERT_EQ(cmd.m_status, command_store::Status::SUCCESS);
    }
    else
    {
        FAIL();
    }
}

TEST_F(CommandStoreTest, DeleteCommandTest)
{
    int initialCount = m_commandStore->GetCount();

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    m_commandStore->DeleteCommand(NINE);

    ASSERT_EQ(m_commandStore->GetCount(), initialCount - 1);
}

TEST_F(CommandStoreTest, GetCommandTest)
{
    m_commandStore->Clear();

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd1(
        FIVE, "Module1", "{CommandTextHERE}", "Parameter1", "Result1", command_store::Status::IN_PROGRESS);
    m_commandStore->StoreCommand(cmd1);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd2(
        NINE, "Module2", "TestValue9", "Parameter2", "Result2", command_store::Status::IN_PROGRESS);
    m_commandStore->StoreCommand(cmd2);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    command_store::Command cmd3(
        ELEVEN, "Module3", "{CommandTextHERE}", "Parameter3", "Result3", command_store::Status::IN_PROGRESS);
    m_commandStore->StoreCommand(cmd3);
    ASSERT_EQ(m_commandStore->GetCount(), 3);

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    std::optional<command_store::Command> retValue = m_commandStore->GetCommand(9);
    if (retValue.has_value())
    {
        const command_store::Command& cmd = retValue.value();
        ASSERT_EQ(cmd.m_id, NINE);
        ASSERT_EQ(cmd.m_module, "Module2");
        ASSERT_EQ(cmd.m_command, "TestValue9");
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers)
    retValue = m_commandStore->GetCommand(ELEVEN);
    if (retValue.has_value())
    {
        const command_store::Command& cmd = retValue.value();
        ASSERT_EQ(cmd.m_id, ELEVEN);
        ASSERT_EQ(cmd.m_module, "Module3");
        ASSERT_EQ(cmd.m_command, "{CommandTextHERE}");
    }
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
