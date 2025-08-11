#pragma once

#include <string>
#include <map>
#include <memory>
#include <functional>
#include <nlohmann/json.hpp>

namespace xeno::ai {

/**
 * @brief AI Integration class for managing API calls to various AI services
 * 
 * Handles integration with:
 * - Xeno AI Cloud (credit-based)
 * - Open Router API (third-party models)
 * - Ollama (local LLMs)
 */
class AIIntegration {
public:
    struct APIConfig {
        std::string endpoint;
        std::string api_key;
        std::map<std::string, std::string> headers;
    };

    struct AIRequest {
        std::string prompt;
        std::string model;
        std::map<std::string, nlohmann::json> parameters;
        std::string operation_type; // e.g., "generative_fill", "code_completion"
    };

    struct AIResponse {
        bool success;
        std::string content;
        int credits_used;
        std::string error_message;
        nlohmann::json metadata;
    };

    enum class AIProvider {
        XenoCloud,
        OpenRouter,
        Ollama
    };

    AIIntegration();
    ~AIIntegration();

    // Configuration
    bool configure(AIProvider provider, const APIConfig& config);
    bool loadConfigFromFile(const std::string& config_path);

    // Credit management (Xeno Labs integration)
    int getCreditBalance();
    bool deductCredits(int amount);
    bool validateCredits(int required_credits);

    // AI operations
    AIResponse generateImage(const AIRequest& request, AIProvider provider = AIProvider::XenoCloud);
    AIResponse processVideo(const AIRequest& request, AIProvider provider = AIProvider::XenoCloud);
    AIResponse processAudio(const AIRequest& request, AIProvider provider = AIProvider::XenoCloud);
    AIResponse completeCode(const AIRequest& request, AIProvider provider = AIProvider::XenoCloud);
    AIResponse chatCompletion(const AIRequest& request, AIProvider provider = AIProvider::XenoCloud);

    // Generic API call
    AIResponse makeAPICall(AIProvider provider, const std::string& endpoint, 
                          const nlohmann::json& payload);

    // Health checks
    bool isProviderAvailable(AIProvider provider);
    std::string getProviderStatus(AIProvider provider);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Credit Wallet manager for Xeno Labs integration
 */
class CreditWallet {
public:
    CreditWallet();
    ~CreditWallet();

    bool authenticate(const std::string& user_token);
    int getBalance();
    bool deductCredits(int amount, const std::string& operation);
    bool addCredits(int amount); // For testing/admin
    
    struct Transaction {
        std::string id;
        std::string operation;
        int credits;
        std::string timestamp;
        bool success;
    };
    
    std::vector<Transaction> getTransactionHistory(int limit = 50);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace xeno::ai