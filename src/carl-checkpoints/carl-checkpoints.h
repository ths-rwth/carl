/**
 * @file
 *
 * A small wrapper that makes using checkpoints easier in carl.
 * 
 *
 * Note that this header should *not* be included if you want to use the carl
 * checkpoints facilities yourself. To do that, include Checkpoints.h and create
 * checkpoints macros like below for your own application.
 * 
 */

#pragma once

#include "config.h"
#include "Checkpoints.h"

namespace carl {
namespace checkpoints {

#ifdef CARL_DEVOPTION_Checkpoints
#define CARL_ADD_CHECKPOINT(channel,description,forced,...) carl::checkpoints::CheckpointVerifier::getInstance().push(channel, description, forced, __VA_ARGS__);
#define CARL_CHECKPOINT(channel,description,...) carl::checkpoints::CheckpointVerifier::getInstance().expect(channel, description, __VA_ARGS__);
#define CARL_CLEAR_CHECKPOINT(channel) carl::checkpoints::CheckpointVerifier::getInstance().clear(channel);
#else
#define CARL_ADD_CHECKPOINT(channel,description,forced,...)
#define CARL_CHECKPOINT(channel,description,...)
#define CARL_CLEAR_CHECKPOINT(channel)
#endif



}
}