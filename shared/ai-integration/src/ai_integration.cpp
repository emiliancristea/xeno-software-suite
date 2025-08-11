#include "ai_integration.h"
#ifdef HAVE_HTTPLIB
#include <httplib.h>
#endif
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif
#include <iostream>
#include <fstream>

namespace xeno::ai {

// Private implementation
class AIIntegration::Impl {
public:
    std::map<AIProvider, APIConfig> configs;
    std::unique_ptr<CreditWallet> wallet;
    
    Impl() : wallet(std::make_unique<CreditWallet>()) {}
};

class CreditWallet::Impl {
public:
    std::string user_token;
    std::string xeno_labs_endpoint = "https://api.xenolabs.ai";
    int cached_balance = 0;
    std::vector<Transaction> transactions;
};

// AIIntegration implementation
AIIntegration::AIIntegration() : pImpl(std::make_unique<Impl>()) {}
AIIntegration::~AIIntegration() = default;

bool AIIntegration::configure(AIProvider provider, const APIConfig& config) {
    pImpl->configs[provider] = config;
    return true;
}

bool AIIntegration::loadConfigFromFile(const std::string& config_path) {
    try {
        std::ifstream file(config_path);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json config;
        file >> config;
        
        // Configure Xeno AI
        if (config.contains("xeno_ai")) {
            APIConfig xeno_config;
            xeno_config.endpoint = config["xeno_ai"]["endpoint"];
            xeno_config.api_key = config["xeno_ai"]["api_key"];
            xeno_config.headers["Authorization"] = "Bearer " + xeno_config.api_key;
            configure(AIProvider::XenoCloud, xeno_config);
        }
        
        // Configure Open Router
        if (config.contains("open_router")) {
            APIConfig router_config;
            router_config.endpoint = config["open_router"]["endpoint"];
            router_config.api_key = config["open_router"]["api_key"];
            router_config.headers["Authorization"] = "Bearer " + router_config.api_key;
            configure(AIProvider::OpenRouter, router_config);
        }
        
        // Configure Ollama
        if (config.contains("ollama")) {
            APIConfig ollama_config;
            ollama_config.endpoint = config["ollama"]["endpoint"];
            configure(AIProvider::Ollama, ollama_config);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
}

int AIIntegration::getCreditBalance() {
    return pImpl->wallet->getBalance();
}

bool AIIntegration::deductCredits(int amount) {
    return pImpl->wallet->deductCredits(amount, "api_call");
}

bool AIIntegration::validateCredits(int required_credits) {
    return getCreditBalance() >= required_credits;
}

AIIntegration::AIResponse AIIntegration::generateImage(const AIRequest& request, AIProvider provider) {
    AIResponse response;
    
    // Check credits for Xeno Cloud
    if (provider == AIProvider::XenoCloud) {
        int required_credits = 3; // Base cost for image generation
        if (!validateCredits(required_credits)) {
            response.success = false;
            response.error_message = "Insufficient credits";
            return response;
        }
    }
    
    // Simulate API call for now
    response.success = true;
    response.content = "Generated image data (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 3 : 0;
    
    if (provider == AIProvider::XenoCloud) {
        deductCredits(response.credits_used);
    }
    
    return response;
}

AIIntegration::AIResponse AIIntegration::processVideo(const AIRequest& request, AIProvider provider) {
    AIResponse response;
    
    if (provider == AIProvider::XenoCloud) {
        int required_credits = 5; // Base cost for video processing
        if (!validateCredits(required_credits)) {
            response.success = false;
            response.error_message = "Insufficient credits";
            return response;
        }
    }
    
    response.success = true;
    response.content = "Processed video data (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 5 : 0;
    
    if (provider == AIProvider::XenoCloud) {
        deductCredits(response.credits_used);
    }
    
    return response;
}

AIIntegration::AIResponse AIIntegration::processAudio(const AIRequest& request, AIProvider provider) {
    AIResponse response;
    
    if (provider == AIProvider::XenoCloud) {
        int required_credits = 2; // Base cost for audio processing
        if (!validateCredits(required_credits)) {
            response.success = false;
            response.error_message = "Insufficient credits";
            return response;
        }
    }
    
    response.success = true;
    response.content = "Processed audio data (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 2 : 0;
    
    if (provider == AIProvider::XenoCloud) {
        deductCredits(response.credits_used);
    }
    
    return response;
}

AIIntegration::AIResponse AIIntegration::completeCode(const AIRequest& request, AIProvider provider) {
    AIResponse response;
    
    if (provider == AIProvider::XenoCloud) {
        int required_credits = 1; // Base cost for code completion
        if (!validateCredits(required_credits)) {
            response.success = false;
            response.error_message = "Insufficient credits";
            return response;
        }
    }
    
    response.success = true;
    response.content = "Code completion suggestion (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 1 : 0;
    
    if (provider == AIProvider::XenoCloud) {
        deductCredits(response.credits_used);
    }
    
    return response;
}

AIIntegration::AIResponse AIIntegration::chatCompletion(const AIRequest& request, AIProvider provider) {
    AIResponse response;
    
    if (provider == AIProvider::XenoCloud) {
        int required_credits = 1;
        if (!validateCredits(required_credits)) {
            response.success = false;
            response.error_message = "Insufficient credits";
            return response;
        }
    }
    
    response.success = true;
    response.content = "Chat response (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 1 : 0;
    
    if (provider == AIProvider::XenoCloud) {
        deductCredits(response.credits_used);
    }
    
    return response;
}

AIIntegration::AIResponse AIIntegration::makeAPICall(AIProvider provider, const std::string& endpoint, 
                                                   const nlohmann::json& payload) {
    AIResponse response;
    
    // For now, return a placeholder response
    // In production, this would make actual HTTP calls using httplib
    response.success = true;
    response.content = "API response (placeholder)";
    response.credits_used = provider == AIProvider::XenoCloud ? 1 : 0;
    
    return response;
}

bool AIIntegration::isProviderAvailable(AIProvider provider) {
    return pImpl->configs.find(provider) != pImpl->configs.end();
}

std::string AIIntegration::getProviderStatus(AIProvider provider) {
    if (!isProviderAvailable(provider)) {
        return "Not configured";
    }
    return "Available";
}

// CreditWallet implementation
CreditWallet::CreditWallet() : pImpl(std::make_unique<Impl>()) {
    // Initialize with some demo credits
    pImpl->cached_balance = 100;
}

CreditWallet::~CreditWallet() = default;

bool CreditWallet::authenticate(const std::string& user_token) {
    pImpl->user_token = user_token;
    return !user_token.empty();
}

int CreditWallet::getBalance() {
    // In production, this would make an API call to Xeno Labs
    return pImpl->cached_balance;
}

bool CreditWallet::deductCredits(int amount, const std::string& operation) {
    if (pImpl->cached_balance >= amount) {
        pImpl->cached_balance -= amount;
        
        // Log transaction
        Transaction transaction;
        transaction.id = "tx_" + std::to_string(pImpl->transactions.size() + 1);
        transaction.operation = operation;
        transaction.credits = -amount;
        transaction.timestamp = "2024-01-01T00:00:00Z"; // Placeholder
        transaction.success = true;
        pImpl->transactions.push_back(transaction);
        
        return true;
    }
    return false;
}

bool CreditWallet::addCredits(int amount) {
    pImpl->cached_balance += amount;
    
    Transaction transaction;
    transaction.id = "tx_" + std::to_string(pImpl->transactions.size() + 1);
    transaction.operation = "credit_purchase";
    transaction.credits = amount;
    transaction.timestamp = "2024-01-01T00:00:00Z"; // Placeholder
    transaction.success = true;
    pImpl->transactions.push_back(transaction);
    
    return true;
}

std::vector<CreditWallet::Transaction> CreditWallet::getTransactionHistory(int limit) {
    std::vector<Transaction> result;
    int count = std::min(limit, static_cast<int>(pImpl->transactions.size()));
    
    for (int i = pImpl->transactions.size() - count; i < static_cast<int>(pImpl->transactions.size()); i++) {
        result.push_back(pImpl->transactions[i]);
    }
    
    return result;
}

} // namespace xeno::ai