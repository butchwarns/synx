/*
PluginParameters.cpp
Copyright (C) 2024 Butch Warns

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "PluginParameters.h"
#include "../external/BDSP/source/utility.h"
#include "../external/BDSP/source/mappings.h"
#include "../external/BDSP/source/decibel.h"

constexpr double VOLUME_MIN = -66.1;
constexpr double VOLUME_MAX = 35.0;
constexpr double VOLUME_OFF_THRESHOLD = VOLUME_MIN + 0.1;

PluginParameters::PluginParameters(juce::AudioProcessor &processor)
    : apvts(processor, nullptr, "parameters", parameter_layout())
{
    volume_norm = apvts.getRawParameterValue("volume");
}

double PluginParameters::volume()
{
    const double gain = denormalise_volume((double)*volume_norm);
    return gain;
}

double PluginParameters::normalise_volume(double gain)
{
    const double db = bdsp::decibel::raw_gain_to_db(gain);

    const auto val_norm = normalise_volume_db(db);

    return val_norm;
}
double PluginParameters::normalise_volume_db(double db)
{
    double val_norm = bdsp::mappings::normalise<double>(db, VOLUME_MIN, VOLUME_MAX);
    val_norm = skew_volume(val_norm);

    return val_norm;
}

double PluginParameters::denormalise_volume(double val_norm)
{
    const auto db = denormalise_volume_db(val_norm);

    const double gain = bdsp::decibel::db_to_raw_gain_off(db, VOLUME_MIN + VOLUME_OFF_THRESHOLD);

    return gain;
}

double PluginParameters::denormalise_volume_db(double val_norm)
{
    val_norm = unskew_volume(val_norm);
    const auto db = bdsp::mappings::linear_norm<double>(val_norm, VOLUME_MIN, VOLUME_MAX);

    return db;
}

String PluginParameters::volume_string_from_value(double value, int max_string_len)
{
    const double val_denorm = denormalise_volume_db(value);

    std::stringstream val_formatted;
    if (val_denorm <= VOLUME_OFF_THRESHOLD)
    {
        // Below threshold, turn off
        val_formatted << "OFF";
    }
    else // Default
    {
        // Format value string to the correct number of decimal places
        const int num_decimal_places = 1;
        const String value_suffix = "dB";
        val_formatted << std::fixed << std::setprecision(num_decimal_places);
        val_formatted << val_denorm << value_suffix;
    }

    String val_formatted_str = val_formatted.str();

    constrain_string_length(val_formatted_str, max_string_len);

    return val_formatted_str;
}

std::optional<double> PluginParameters::volume_value_from_string(const String &string)
{
    if (string.toLowerCase() == "off")
    {
        return 0.0;
    }
    else
    {
        if (string.endsWithIgnoreCase("dB"))
        {
            string.dropLastCharacters(2);
        }

        try
        {
            double value = 0.0;
            value = std::stod(string.toStdString());
            value = normalise_volume_db(value);
            return value;
        }
        catch (const std::invalid_argument &e)
        {
        }
        catch (const std::out_of_range &e)
        {
        }
    }

    return std::nullopt;
}

double PluginParameters::skew_volume(double val_norm)
{
    const double zero_norm = bdsp::mappings::normalise(0.0, VOLUME_MIN, VOLUME_MAX);

    if (val_norm <= zero_norm)
    {
        return bdsp::mappings::linear(val_norm, 0.0, zero_norm, 0.0, 0.5);
    }

    return bdsp::mappings::linear(val_norm, zero_norm, 1.0, 0.5, 1.0);
}

double PluginParameters::unskew_volume(double val_skewed)
{
    const double zero_norm = bdsp::mappings::normalise(0.0, VOLUME_MIN, VOLUME_MAX);

    if (val_skewed <= 0.5)
    {
        return bdsp::mappings::linear(val_skewed, 0.0, 0.5, 0.0, zero_norm);
    }

    return bdsp::mappings::linear(val_skewed, 0.5, 1.0, zero_norm, 1.0);
}

Apvts::ParameterLayout PluginParameters::parameter_layout()
{
    Apvts::ParameterLayout layout;

    typedef juce::AudioProcessorParameterGroup ParameterGroup;

    auto sliders_grp = std::make_unique<ParameterGroup>("sliders", "SLIDERS", "|");
    const auto volume_id = ParameterID{"volume", 1};
    const auto volume_default = (float)normalise_volume_db(0.0);
    sliders_grp->addChild(std::make_unique<juce::AudioParameterFloat>(volume_id, "VOLUME", NormalisableRange<float>(0.0f, 1.0f, 0.0000001f), volume_default, "", AudioProcessorParameter::genericParameter, volume_string_from_value));

    layout.add(std::move(sliders_grp));

    return layout;
}

void PluginParameters::constrain_string_length(String &s, int max_len)
{
    const int len = s.length();
    if (len > max_len)
    {
        s = s.dropLastCharacters(len - max_len);
    }
}