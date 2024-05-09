/*
PluginParameters.h
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

#ifndef PLUGIN_PARAMETERS_H
#define PLUGIN_PARAMETERS_H

#include <JuceHeader.h>
#include "typedefs.h"

class PluginParameters
{
public:
    explicit PluginParameters(juce::AudioProcessor &processor);

    Apvts apvts;

    double volume();
    static inline double normalise_volume(double gain);
    static inline double normalise_volume_db(double db);
    static inline double denormalise_volume(double val_norm);
    static inline double denormalise_volume_db(double val_norm);
    static String volume_string_from_value(double value, int max_string_len);
    static std::optional<double> volume_value_from_string(const String &string);
    static inline double skew_volume(double val_norm);
    static inline double unskew_volume(double val_skewed);

private:
    static Apvts::ParameterLayout parameter_layout();

    std::atomic<float> *volume_norm;

    static void constrain_string_length(String &s, int max_len);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginParameters)
};

#endif // PLUGIN_PARAMETERS_H