// ------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved.
// Licensed under the MIT License (MIT). See License.txt in the repo root for license information.
// ------------------------------------------------------------

#include "stdafx.h"

using namespace Common;
using namespace ServiceModel;
using namespace std;

using namespace Management::FaultAnalysisService;

StringLiteral const TraceComponent("NodeTransitionProgress");

NodeTransitionProgress::NodeTransitionProgress()
: state_(TestCommandProgressState::Invalid)
, result_()
{
}

NodeTransitionProgress::NodeTransitionProgress(NodeTransitionProgress && other)
: state_(move(other.state_))
, result_(move(other.result_))
{
}

NodeTransitionProgress & NodeTransitionProgress::operator=(NodeTransitionProgress && other)
{
    if (this != &other)
    {
        state_ = move(other.state_);
        result_ = move(other.result_);
    }

    return *this;
}

NodeTransitionProgress::NodeTransitionProgress(
    TestCommandProgressState::Enum state,
    std::shared_ptr<NodeTransitionResult> && result)
    : state_(state)
    , result_(move(result))
{
}

Common::ErrorCode NodeTransitionProgress::FromPublicApi(__in FABRIC_NODE_TRANSITION_PROGRESS const & publicProgress)
{
    state_ = TestCommandProgressState::FromPublicApi(publicProgress.State);

    NodeTransitionResult result;
    auto error = result.FromPublicApi(*publicProgress.Result);

    if (!error.IsSuccess())
    {
        return error;
    }

    result_ = make_shared<NodeTransitionResult>(result);

    return ErrorCodeValue::Success;
}

void NodeTransitionProgress::ToPublicApi(
    __in ScopedHeap & heap,
    __out FABRIC_NODE_TRANSITION_PROGRESS & progress) const
{        
    if (result_)
    {
        auto resultPtr = heap.AddItem<FABRIC_NODE_TRANSITION_RESULT>();
        result_->ToPublicApi(heap, *resultPtr);
        progress.Result = resultPtr.GetRawPointer();
    }

    progress.State = TestCommandProgressState::ToPublicApi(state_);
}

