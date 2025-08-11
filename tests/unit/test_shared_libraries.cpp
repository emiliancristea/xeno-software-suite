#include <gtest/gtest.h>
#include "../../shared/ai-integration/include/ai_integration.h"
#include "../../shared/utils/include/utils.h"

using namespace xeno::ai;
using namespace xeno::utils;

class AIIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        ai_integration = std::make_unique<AIIntegration>();
    }

    std::unique_ptr<AIIntegration> ai_integration;
};

TEST_F(AIIntegrationTest, InitialCreditBalance) {
    // Test that initial credit balance is positive
    int balance = ai_integration->getCreditBalance();
    EXPECT_GT(balance, 0);
}

TEST_F(AIIntegrationTest, CreditValidation) {
    // Test credit validation
    EXPECT_TRUE(ai_integration->validateCredits(1));
    EXPECT_FALSE(ai_integration->validateCredits(10000)); // Assuming we don't have 10k credits
}

TEST_F(AIIntegrationTest, CreditDeduction) {
    int initial_balance = ai_integration->getCreditBalance();
    bool success = ai_integration->deductCredits(1);
    int new_balance = ai_integration->getCreditBalance();
    
    EXPECT_TRUE(success);
    EXPECT_EQ(new_balance, initial_balance - 1);
}

TEST_F(AIIntegrationTest, ImageGeneration) {
    AIIntegration::AIRequest request;
    request.prompt = "Test image generation";
    request.operation_type = "test";
    
    auto response = ai_integration->generateImage(request);
    EXPECT_TRUE(response.success);
    EXPECT_GT(response.credits_used, 0);
    EXPECT_FALSE(response.content.empty());
}

TEST_F(AIIntegrationTest, CodeCompletion) {
    AIIntegration::AIRequest request;
    request.prompt = "int main() {";
    request.operation_type = "code_completion";
    
    auto response = ai_integration->completeCode(request);
    EXPECT_TRUE(response.success);
    EXPECT_GE(response.credits_used, 0);
}

class UtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        config_manager = &ConfigManager::getInstance();
    }

    ConfigManager* config_manager;
};

TEST_F(UtilsTest, ConfigManager) {
    config_manager->setString("test_key", "test_value");
    EXPECT_EQ(config_manager->getString("test_key"), "test_value");
    
    config_manager->setInt("test_int", 42);
    EXPECT_EQ(config_manager->getInt("test_int"), 42);
    
    config_manager->setBool("test_bool", true);
    EXPECT_TRUE(config_manager->getBool("test_bool"));
}

TEST_F(UtilsTest, Logger) {
    Logger& logger = Logger::getInstance();
    
    // Test that logging doesn't crash
    logger.info("Test info message");
    logger.warning("Test warning message");
    logger.error("Test error message");
    logger.debug("Test debug message");
    
    // Test level setting
    logger.setLevel(Logger::ERROR);
    logger.info("This should not appear");
    logger.error("This should appear");
}

TEST(PlatformTest, OSDetection) {
    Platform::OS os = Platform::getOS();
    EXPECT_NE(os, Platform::Unknown);
    
    std::string os_string = Platform::getOSString();
    EXPECT_FALSE(os_string.empty());
    EXPECT_NE(os_string, "Unknown");
}

TEST(PlatformTest, PathOperations) {
    std::string app_data = Platform::getAppDataPath();
    EXPECT_FALSE(app_data.empty());
    
    std::string temp_path = Platform::getTempPath();
    EXPECT_FALSE(temp_path.empty());
}

class CreditWalletTest : public ::testing::Test {
protected:
    void SetUp() override {
        wallet = std::make_unique<CreditWallet>();
        wallet->authenticate("test_token");
    }

    std::unique_ptr<CreditWallet> wallet;
};

TEST_F(CreditWalletTest, Authentication) {
    EXPECT_TRUE(wallet->authenticate("valid_token"));
    EXPECT_TRUE(wallet->authenticate("")); // Empty token should still work for testing
}

TEST_F(CreditWalletTest, BalanceOperations) {
    int initial_balance = wallet->getBalance();
    EXPECT_GT(initial_balance, 0);
    
    // Test adding credits
    wallet->addCredits(10);
    EXPECT_EQ(wallet->getBalance(), initial_balance + 10);
    
    // Test deducting credits
    bool success = wallet->deductCredits(5, "test_operation");
    EXPECT_TRUE(success);
    EXPECT_EQ(wallet->getBalance(), initial_balance + 5);
}

TEST_F(CreditWalletTest, TransactionHistory) {
    wallet->deductCredits(1, "test_operation");
    wallet->addCredits(5);
    
    auto transactions = wallet->getTransactionHistory(10);
    EXPECT_GE(transactions.size(), 2);
    
    // Check transaction properties
    for (const auto& transaction : transactions) {
        EXPECT_FALSE(transaction.id.empty());
        EXPECT_FALSE(transaction.operation.empty());
        EXPECT_NE(transaction.credits, 0);
        EXPECT_TRUE(transaction.success);
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}