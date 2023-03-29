#include <fstream>
#include <yaml-cpp/yaml.h>

#include "libzork/story/node.hh"
#include "libzork/story/store.hh"
#include "libzork/story/story.hh"

namespace story
{
    Story::Story(const fs::path &path)
    {
        std::ifstream file(path);
        const auto doc = YAML::Load(file);

        // Create the store
        store_ = std::make_unique<Store>();

        // Create the nodes
        for (const auto& node : doc["story"])
        {
            const auto name = node["name"].as<std::string>();
            const auto text = node["text"].as<std::string>();
            Node::Node noeud_actuel(std::make_unique<Node>(name, text));
            nodes_.push_back(noeud_actuel);
            for(const auto& choix : doc["choices"])
            {
                const auto text_du_choix = node["text"].as<std::string>();
                const auto target_du_choix = node["target"].as<std::string>();
                // chercher dans la liste de nodes_, si un node a le nom target du choix
                for(const auto & noeud : nodes_)
                {
                    if(noeud.get_name() == target_du_choix)
                    {
                         // creation de la liste de choix
                choices::choices un_choix(text_du_choix,noeud);
                noeud_actuel.choices.push_back(un_choix);
                    }
                }
            }
        }
    }

    const std::string& Story::get_title() const
    {
        return nodes_.front()->get_text();
    }

    const Node* Story::get_current() const
    {
        return ;
    }

    void Story::set_current(const Node* node)
    {
//        store_ = node;
    }



    std::ostream& operator<<(std::ostream& os, const Story& story)
    {
        os << "digraph story {\n";
        for (const auto& node : story.nodes_)
        {
            const auto& text = node->get_text();
            os << '"' << id << "\" [label=\"" << text << "\"];" << std::endl;

            for (const auto& choice : node->get_choices())
            {
                const auto& choice_text = choice.text;
                //todo add bla bla entre fleches
                os << '"'<<  " -> \"" << << choice_text << "\"];\n";
            }
        }
        os << "}\n";
        return os;
    }

    Node* Story::get_node(const std::string& id)
    {
        for (const auto& node : nodes_)
        {
            if (node->get_id() == id)
            {
                return node.get();
            }
        }
        return nullptr;
    }

    const Node* Story::get_node(const std::string& id) const
    {
        for (const auto& node : nodes_)
        {
            if (node->get_id() == id)
            {
                return node.get();
            }
        }
        return nullptr;
    }
} // namespace story
