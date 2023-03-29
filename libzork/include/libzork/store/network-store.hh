#pragma once

#include <httplib.h>

#include "libzork/store/store.hh"
#include "libzork/story/story.hh"

class NetworkStore : public Store
{
public:
    NetworkStore(const std::string& url, const std::string& username,
                 const story::Story& story);

    const story::Node* get_active_node() const override;
    void set_active_node(const story::Node* node) override;

    bool has_variable(const std::string& name) const override;
    int get_variable(const std::string& name) const override;
    void set_variable(const std::string& name, int value) override;
};
