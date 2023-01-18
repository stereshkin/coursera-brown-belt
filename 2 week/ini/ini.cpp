#include "ini.h"

namespace Ini {
    Section &Document::AddSection(std::string name) {
        if (sections.find(name) == end(sections)) {
            sections[name] = {};
        }
        return sections[name];
    }

    const Section &Document::GetSection(const std::string &name) const {
        return sections.at(name);
    }

    size_t Document::SectionCount() const {
        return sections.size();
    }

    Document Load(std::istream& input) {
        Document document;

        std::string line;
        while (getline(input, line)) {

            if (line.back() == ']' and line.front() == '[') {
                std::string section = line.substr(1, line.size() - 2);
                Section& current_section = document.AddSection(section);
                getline(input, line);
                while (!line.empty()) {
                    std::string key, value;
                    auto pos = line.find('=');
                    key = line.substr(0, pos);
                    value = line.substr(pos+1, std::string::npos);
                    current_section.insert({key, value});
                    getline(input, line);
                }

            }


        }
        return document;
    }
}
