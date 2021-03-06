/// Author: Ralph Ridley
/// Date 29/10/18
/// Purpose: Definitions for Validation.h
#include "Validation.h"

using namespace QZL;

const std::array<const char*, 1> Validation::kLayers = { "VK_LAYER_LUNARG_standard_validation" }; 
const std::array<const char*, 1> Validation::kExtensions = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
bool Validation::tryEnableSuccess = false;

Validation::Validation(const VkInstance instance, const VkDebugUtilsMessageSeverityFlagBitsEXT severityFlag)
	: cInstance_(instance), severityFlag_(severityFlag), callbackHandle_(VK_NULL_HANDLE)
{
	EXPECTS(instance != VK_NULL_HANDLE);
	if (tryEnableSuccess) {
		// Basic validation
		VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = Validation::callback;
		createInfo.pUserData = this;
		auto createFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(cInstance_, "vkCreateDebugUtilsMessengerEXT");
		DEBUG_IFERR((createFunction == nullptr || createFunction(cInstance_, &createInfo, nullptr, &callbackHandle_) != VK_SUCCESS),
			"Warning: Validation callback setup failed.");
		DEBUG_OUT("Validation is enabled");
	}
}

Validation::~Validation()
{
	if (callbackHandle_ != VK_NULL_HANDLE) {
		auto destroyFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(cInstance_, "vkDestroyDebugUtilsMessengerEXT");
		if (destroyFunction != nullptr)
			destroyFunction(cInstance_, callbackHandle_, nullptr);
	}
}

bool Validation::tryEnable(std::vector<const char*>& extensions, uint32_t& enabledLayerCount, const char* const*& ppEnabledLayerNames)
{
	enabledLayerCount = 0;
	ppEnabledLayerNames = NULL;
#ifndef ENABLE_VALIDATION
	return false;
#else
	// Check the extension is available with this Vulkan implementation
	auto availableExtensions = obtainVkData<VkExtensionProperties>(vkEnumerateInstanceExtensionProperties, "");
	bool found = false;
	for (auto ext : kExtensions) {
		for (const auto& extension : availableExtensions) {
			if (strcmp(ext, extension.extensionName) == 0) {
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	// Check the validation layers are available.
	auto layerProperties = obtainVkData<VkLayerProperties>(vkEnumerateInstanceLayerProperties);
	for (auto layer : kLayers) {
		bool found = false;
		for (const auto& property : layerProperties) {
			if (strcmp(layer, property.layerName) == 0) {
				found = true;
				break;
			}
		}
		if(!found) 
			return false; 
	}
	// Only now it is confirmed may the validation be implemented
	enabledLayerCount = static_cast<uint32_t>(kLayers.size());
	ppEnabledLayerNames = kLayers.data();
	for (auto ext : kExtensions) {
		extensions.push_back(ext);
	}
	tryEnableSuccess = true;
	return true;
#endif
}

VKAPI_ATTR VkBool32 VKAPI_CALL Validation::callback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, 
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	if (severity >= reinterpret_cast<Validation*>(pUserData)->severityFlag_) {
		DEBUG_OUT("Validation layer (severity " << severity <<"): " << pCallbackData->pMessage);
	}
	return VK_FALSE;
}
