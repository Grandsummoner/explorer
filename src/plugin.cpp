#include "plugin.hpp"

Plugin* pluginInstance;

void init(Plugin* p) {
    pluginInstance = p;

    // Register Explorer module
    p->addModel(modelExplorer);
}
