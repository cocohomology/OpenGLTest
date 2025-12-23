#pragma once
#include <cstdint>
#include<string>
#include<memory>
#include <unordered_map>

#include<IComponent.h>
#include<entity.h>


// 组件管理器：存储所有实体的组件
class ComponentManager {
private:
    // 组件类型 -> 实体ID -> 组件实例
    std::unordered_map<std::string, std::unordered_map<EntityID, std::unique_ptr<IComponent>>> components;
public:
    // 添加组件到实体
    template<typename T>
    void addComponent(EntityID entity, T component) {
        static_assert(std::is_base_of<IComponent, T>::value, "T must inherit from IComponent");
        std::string typeName = typeid(T).name();
        components[typeName][entity] = std::make_unique<T>(component);
    }

    // 获取实体的组件（不存在返回nullptr）
    template<typename T>
    T* getComponent(EntityID entity) {
        std::string typeName = typeid(T).name();
        if (components.count(typeName) && components[typeName].count(entity)) {
            return static_cast<T*>(components[typeName][entity].get());
        }
        return nullptr;
    }

    // 移除实体的组件
    template<typename T>
    void removeComponent(EntityID entity) {
        std::string typeName = typeid(T).name();
        if (components.count(typeName)) {
            components[typeName].erase(entity);
        }
    }

    // 获取拥有指定组件的所有实体
    template<typename... Ts>
    std::vector<EntityID> getEntitiesWithComponents() {
        std::vector<EntityID> result;
        if (sizeof...(Ts) == 0) return result;

        // 先获取第一个组件的实体列表
        std::vector<EntityID> candidates;
        std::string firstType = typeid(std::tuple_element_t<0, std::tuple<Ts...>>).name();
        if (components.count(firstType)) {
            for (auto& pair : components[firstType]) {
                candidates.push_back(pair.first);
            }
        }

        // 筛选同时拥有所有组件的实体
        for (EntityID entity : candidates) {
            bool hasAll = true;
            // 检查剩余组件
            auto check = [&](auto&&... ts) {
                (void)std::initializer_list<int>{
                    (hasAll = hasAll && (getComponent<std::decay_t<decltype(ts)>>(entity) != nullptr), 0)...
                };
                };
            check(Ts{}...);
            if (hasAll) {
                result.push_back(entity);
            }
        }
        return result;
    }
};

