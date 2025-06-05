#include "ResourceManager.h"
#include <algorithm>
#include <string.h>
#include <stdio.h>
#include "lvgl.h"

#define ResourceManager_DEBUG
#ifdef ResourceManager_DEBUG
#define RES_LOG_INFO(format, ...) printf("[ResourceManager] [Info] " format, ##__VA_ARGS__)
#define RES_LOG_WARN(format, ...) printf("[ResourceManager] [Warn] " format, ##__VA_ARGS__)
#define RES_LOG_ERROR(format, ...) printf("[ResourceManager] [Error] " format, ##__VA_ARGS__)
#else
#define RES_LOG_INFO(...)
#define RES_LOG_WARN(...)
#define RES_LOG_ERROR(...)
#endif

ResourceManager::ResourceManager()
{
    DefaultPtr = nullptr;
}

ResourceManager::~ResourceManager()
{
}

/**
  * @brief  Search resource node based on name
  * @param  name: Resource Name
  * @param  node: Pointer to the resource node
  * @retval Return true if the search is successful
  */
bool ResourceManager::SearchNode(const char* name, ResourceNode_t* node)
{
    for(auto iter : NodePool)
    {
        if (strcmp(name, iter.name) == 0)
        {
            *node = iter;
            return true;
        }
    }
    return false;
}

/**
  * @brief  Add resources to the resource pool
  * @param  name: Resource Name
  * @param  ptr: Pointer to the resource
  * @retval Return true if the addition is successful
  */
bool ResourceManager::AddResource(const char* name, void* ptr)
{
    ResourceNode_t node;
    if (SearchNode(name, &node))
    {
        RES_LOG_WARN("%s was register\r\n", name);
        return false;
    }

    node.name = name;
    node.ptr = ptr;
    NodePool.push_back(node);

    RES_LOG_INFO("%s[0x%p] add success\r\n", node.name, node.ptr);

    return true;
}

/**
  * @brief  Remove resources from the resource pool
  * @param  name: Resource Name
  * @retval Return true if the removal is successful
  */
bool ResourceManager::RemoveResource(const char* name)
{
    ResourceNode_t node;
    if(!SearchNode(name, &node))
    {
        RES_LOG_ERROR("%s was not found\r\n", name);
        return false;
    }

    auto iter = std::find(NodePool.begin(), NodePool.end(), node);

    if (iter == NodePool.end())
    {
        RES_LOG_ERROR("%s was not found\r\n", name);
        return false;
    }

    NodePool.erase(iter);

    RES_LOG_INFO("%s remove success\r\n", name);

    return true;
}

/**
  * @brief  Get resource address
  * @param  name: Resource Name
  * @retval If the acquisition is successful, return the address of the resource, otherwise return the default resource
  */
void* ResourceManager::GetResource(const char* name)
{
    ResourceNode_t node;

    if(!SearchNode(name, &node))
    {
        RES_LOG_WARN("%s was not found, return default[0x%p]\r\n", name, DefaultPtr);
        return DefaultPtr;
    }

    RES_LOG_INFO("%s[0x%p] was found\r\n", name, node.ptr);

    return node.ptr;
}

/**
  * @brief  Set default resources
  * @param  ptr: Pointer to the default resource
  * @retval None
  */
void ResourceManager::SetDefault(void* ptr)
{
    DefaultPtr = ptr;
    RES_LOG_INFO("set [0x%p] to default\r\n", DefaultPtr);
}
