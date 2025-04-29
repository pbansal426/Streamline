<<<<<<< HEAD
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
=======
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
>>>>>>> 732919b (Updated MVP, fixed readme, tweaked AI settings)
