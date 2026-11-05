#ifndef MATERIALS_HPP
#define MATERIALS_HPP

#include <string>
#include <unordered_map>
#include "camera_light.hpp"

class MaterialLibrary {
public:
    static Material getMaterial(const std::string& name) {
        // Structuur: Ambient(r,g,b), Diffuse(r,g,b), Specular(r,g,b), Shininess (shine * 128.0), Opacity
        static const std::unordered_map<std::string, Material> presets = {
            // EDELSTENEN & METALEN
            {"EMERALD",   {{0.0215f, 0.1745f, 0.0215f},  {0.07568f, 0.61424f, 0.07568f}, {0.633f, 0.727811f, 0.633f},    76.8f, 1.0f}},
            {"JADE",      {{0.135f, 0.2225f, 0.1575f},   {0.54f, 0.89f, 0.63f},          {0.316228f, 0.316228f, 0.316228f}, 12.8f, 1.0f}},
            {"OBSIDIAN",  {{0.05375f, 0.05f, 0.06625f},  {0.18275f, 0.17f, 0.22525f},    {0.332741f, 0.328634f, 0.346435f}, 38.4f, 1.0f}},
            {"PEARL",     {{0.25f, 0.20725f, 0.20725f},  {1.0f, 0.829f, 0.829f},         {0.296648f, 0.296648f, 0.296648f}, 11.264f, 1.0f}},
            {"RUBY",      {{0.1745f, 0.01175f, 0.01175f},{0.61424f, 0.04136f, 0.04136f}, {0.727811f, 0.626959f, 0.626959f}, 76.8f, 1.0f}},
            {"TURQUOISE", {{0.1f, 0.18725f, 0.1745f},    {0.396f, 0.74151f, 0.69102f},   {0.297254f, 0.30829f, 0.306678f}, 12.8f, 1.0f}},
            {"BRASS",     {{0.329412f, 0.223529f, 0.027451f}, {0.780392f, 0.568627f, 0.113725f}, {0.992157f, 0.941176f, 0.807843f}, 27.9f, 1.0f}},
            {"BRONZE",    {{0.2125f, 0.1275f, 0.054f},   {0.714f, 0.4284f, 0.18144f},    {0.393548f, 0.271906f, 0.166721f}, 25.6f, 1.0f}},
            {"CHROME",    {{0.25f, 0.25f, 0.25f},        {0.4f, 0.4f, 0.4f},             {0.774597f, 0.774597f, 0.774597f}, 76.8f, 1.0f}},
            {"COPPER",    {{0.19125f, 0.0735f, 0.0225f}, {0.7038f, 0.27048f, 0.0828f},   {0.256777f, 0.137622f, 0.086014f}, 12.8f, 1.0f}},
            {"GOLD",      {{0.24725f, 0.1995f, 0.0745f}, {0.75164f, 0.60648f, 0.22648f}, {0.628281f, 0.555802f, 0.366065f}, 51.2f, 1.0f}},
            {"SILVER",    {{0.19225f, 0.19225f, 0.19225f},{0.50754f, 0.50754f, 0.50754f},{0.508273f, 0.508273f, 0.508273f}, 51.2f, 1.0f}},

            // PLASTICS
            {"BLACK_PLASTIC", {{0.0f, 0.0f, 0.0f}, {0.01f, 0.01f, 0.01f}, {0.50f, 0.50f, 0.50f}, 32.0f, 1.0f}},
            {"CYAN_PLASTIC",  {{0.0f, 0.1f, 0.06f}, {0.0f, 0.5098f, 0.5098f}, {0.5019f, 0.5019f, 0.5019f}, 32.0f, 1.0f}},
            {"RED_PLASTIC",   {{0.0f, 0.0f, 0.0f}, {0.5f, 0.0f, 0.0f}, {0.7f, 0.6f, 0.6f}, 32.0f, 1.0f}},

            // RUBBERS
            {"RED_RUBBER",    {{0.05f, 0.0f, 0.0f}, {0.5f, 0.4f, 0.4f}, {0.7f, 0.04f, 0.04f}, 10.0f, 1.0f}}
        };

        auto it = presets.find(name);
        if (it != presets.end()) {
            return it->second;
        }

        return Material{};
    }
};

#endif // MATERIALS_HPP