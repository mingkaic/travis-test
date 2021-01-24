
#ifndef PKG_ESTD_CONFIG_HPP
#define PKG_ESTD_CONFIG_HPP

#include <functional>

#include "estd/contain.hpp"

namespace estd
{

template <typename K=std::string, typename HASH=std::hash<K>>
struct iConfig
{
	virtual ~iConfig (void) = default;

	virtual std::vector<K> get_keys (void) const = 0;

	virtual bool has_key (const K& cfg_key) const = 0;

	virtual void* get_obj (const K& cfg_key) = 0;
};

using EntryDelF = std::function<void(void*)>;

struct ConfigEntry
{
	ConfigEntry (void* data, EntryDelF deletion = EntryDelF()) :
		data_(data), deletion_(deletion) {}

	~ConfigEntry (void)
	{
		if (deletion_)
		{
			deletion_(data_);
		}
	}

	ConfigEntry (ConfigEntry&& other) :
		data_(std::move(other.data_)), deletion_(std::move(other.deletion_)) {}

	ConfigEntry& operator = (ConfigEntry&& other)
	{
		if (this != &other)
		{
			data_ = std::move(other.data_);
			deletion_ = std::move(other.deletion_);
		}
		return *this;
	}

	ConfigEntry (const ConfigEntry& other) = delete;

	ConfigEntry& operator = (const ConfigEntry& other) = delete;

	void* data_;

	EntryDelF deletion_;
};

using EntryptrT = std::shared_ptr<ConfigEntry>;

template <typename K=std::string, typename HASH=std::hash<K>>
struct ConfigMap final : public iConfig<K,HASH>
{
	std::vector<K> get_keys (void) const override
	{
		std::vector<K> names;
		names.reserve(entries_.size());
		std::transform(entries_.begin(), entries_.end(),
			std::back_inserter(names),
			[](const std::pair<K,EntryptrT>& entry)
			{
				return entry.first;
			});
		return names;
	}

	bool has_key (const K& cfg_key) const override
	{
		return has(entries_, cfg_key);
	}

	void* get_obj (const K& cfg_key) override
	{
		if (has(entries_, cfg_key))
		{
			auto& entry = entries_.at(cfg_key);
			return entry->data_;
		}
		return nullptr;
	}

	template <typename T>
	void add_entry (const K& cfg_key,
		std::function<T*()> init = []()
		{
			return new T();
		},
		std::function<void(void*)> del = [](void* ptr)
		{
			delete static_cast<T*>(ptr);
		})
	{
		if (false == estd::has(entries_, cfg_key))
		{
			entries_.emplace(cfg_key,
				std::make_shared<ConfigEntry>(init(), del));
		}
	}

	void rm_entry (const K& cfg_key)
	{
		if (has(entries_, cfg_key))
		{
			entries_.erase(cfg_key);
		}
	}

private:

	std::unordered_map<K,EntryptrT,HASH> entries_;
};

}

#endif // PKG_ESTD_CONFIG_HPP
