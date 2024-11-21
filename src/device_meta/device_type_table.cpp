// Created By Rokas Paulauskas on 19/11/2024
#include "alice/alice.hpp"

void alice::DeviceTypeTable::update_type(const uint32_t id, PeerType type)
{
    try
    {
        type_table_[id] = type;
    }
    catch (const std::exception &e)
    {
        Logger::log(LogLevel::ERROR, "Error updating type: " + std::string(e.what()));
    }
}
PeerType alice::DeviceTypeTable::get_type(const uint32_t id) const
{
    if (type_table_.find(id) != type_table_.end())
    {
        return type_table_.at(id);
    }
    else
    {
        throw std::runtime_error("ID not found in type table.");
    }
}
void alice::DeviceTypeTable::remove_type(const uint32_t id)
{
    type_table_.erase(id);
}
std::unordered_map<uint32_t, PeerType> alice::DeviceTypeTable::get_table() const
{
    return type_table_;
}