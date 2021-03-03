// Aufgabe 1
// Loris Wilwert, Raphael Julien Ebner, Catherine Wampach

#include "frequencysource.hh"
#include <map>
#include <unordered_map>
#include <cctype>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>


/* --------------- Punkt a) --------------- */
template <typename Map>
class LetterFrequencies {
    public:
        // Processes all letters obtained from source
        template <typename Source>
        void readData(Source &source) {
            while (true)
            {
                auto data = source.next();
                if (not data.second)
                    break;

                _map[data.first] += 1;
            }
        }

        // does the statistics and prints to stdout
        void printStatistics() {
            std::size_t total = 0;

            for (auto entry : _map) total += entry.second;

            for (auto entry : _map)
                std::cout << entry.first << ": "
                        << static_cast<double>(entry.second) / total << std::endl;
        }
        
    private:
        Map _map;
};


/* --------------- Punkt c) --------------- */
template<typename Map>
class AnalysisPlugin {
    public:
        using Data = typename Map::key_type;

        // always add virtual destructor
        virtual ~AnalysisPlugin() {}

        // returns the name of the plugin
        virtual std::string name() const = 0;

        // does some statistics on the map and prints it to stdout
        virtual void printStatistics(const Map& map) {
        // do nothing by default
        }
};


template<typename Map>
class PrintFrequencies : public AnalysisPlugin<Map> {
    std::string name() const override {
        return "PrintFrequencies";
    }

    void printStatistics(const Map& map) override {
        std::size_t total = 0;

        for (auto entry : map) total += entry.second;

        for (auto entry : map)
            std::cout << entry.first << ": "
                    << static_cast<double>(entry.second) / total << std::endl;
    }
};


template<typename Map>
class PrintTotalCount : public AnalysisPlugin<Map> {
    std::string name() const override {
        return "PrintTotalCount";
    }

    void printStatistics(const Map& map) override {
        std::size_t total = 0;

        for (auto entry : map) total += entry.second;
        std::cout << total << std::endl;
    }
};


/* --------------- Punkt b) --------------- */
template <typename Map, typename Filter>
class Frequencies {
    public:
        // Constructor
        Frequencies(Filter& filter) : _filter(filter) {}

        /* --------------- Punkt d) --------------- */
        // abstract Plugin base type
        using Plugin = AnalysisPlugin<Map>;

        // add a new plugin
        void addPlugin(const std::shared_ptr<Plugin> plugin) {  
            _plugins.push_back(plugin);
        }

        // Processes all letters obtained from source
        template <typename Source>
        void readData(Source &source) {
            while (true)
            {
                auto data = source.next();
                if (not data.second)
                    break;

                if (not _filter.remove(data.first)) 
                    _map[_filter.transform(data.first)] += 1;  
            }
        }

        // does the statistics and prints to stdout
        void printStatistics() {
            for (auto& plugin : _plugins) {
                plugin->name();
                plugin->printStatistics(_map);
            }
        }

    private:
        Map _map;
        Filter _filter;
        using Data = typename Map::key_type;
        std::vector<std::shared_ptr<Plugin>> _plugins;
};


class FilterLetter {
    public:
        char transform(const char& data) {
            return std::toupper(data);
        }

        bool remove(const char& data) {
            return not std::isalpha(data);
        }
};


class FilterWord {
    public:
        std::string transform(const std::string& data) {
			std::string result = data;
            for (auto& s : result)
				s = std::toupper(s);
			return result;
        }

        bool remove(const std::string& data) {
            for (auto& s : data)
                if (not std::isalpha(s))
                    return true;
            return false;
        }
};


int main (int argc, char *argv[]) {
    // Zähle die Wörter
    using WordMap = std::unordered_map<std::string,int>;
    FilterWord filter;
    Frequencies<WordMap, FilterWord> frequencies(filter);
    frequencies.addPlugin(std::make_shared<PrintFrequencies<WordMap>>());
    frequencies.addPlugin(std::make_shared<PrintTotalCount<WordMap>>());
    
    /*
    // Zähle die Buchstaben
    using LetterMap = std::map<char,int>;
    FilterLetter filter;
    Frequencies<LetterMap, FilterLetter> frequencies(filter);
    frequencies.addPlugin(std::make_shared<PrintFrequencies<LetterMap>>());
    frequencies.addPlugin(std::make_shared<PrintTotalCount<LetterMap>>());
    */


    /* 
    // Variante 1: Eigenen Text in der Konsole
    auto source = streamWordSource(std::cin);
    //auto source = streamLetterSource(std::cin);
    frequencies.readData(source);
    */

    // Variante 2: Dateieingabe in der Konsole
    for (int i = 1 ; i < argc ; ++i) {
        std::ifstream f(argv[i]);
        auto source = streamWordSource(f);
        //auto source = streamLetterSource(f);
        frequencies.readData(source);
    }

    frequencies.printStatistics();
    
    return 0;
}