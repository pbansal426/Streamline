/*
==============================================================================

  This file contains the basic framework code for a JUCE plugin.

==============================================================================
*/

#include <JuceHeader.h>
#include "StreamlineProcessor.h"

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
  return new StreamlineProcessor();
}

