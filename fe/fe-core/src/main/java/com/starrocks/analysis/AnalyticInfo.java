// Copyright 2021-present StarRocks, Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// This file is based on code available under the Apache license here:
//   https://github.com/apache/incubator-doris/blob/master/fe/fe-core/src/main/java/org/apache/doris/analysis/AnalyticInfo.java

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

package com.starrocks.analysis;

import com.google.common.base.MoreObjects;
import com.google.common.base.Preconditions;
import com.google.common.collect.Lists;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import java.util.ArrayList;
import java.util.List;

/**
 * Encapsulates the analytic functions found in a single select block plus
 * the corresponding analytic result tuple and its substitution map.
 */
public final class AnalyticInfo extends AggregateInfoBase {
    private static final Logger LOG = LoggerFactory.getLogger(AnalyticInfo.class);

    // All unique analytic exprs of a select block. Used to populate
    // super.aggregateExprs_ based on AnalyticExpr.getFnCall() for each analytic expr
    // in this list.
    private final ArrayList<Expr> analyticExprs_;

    // Intersection of the partition exps of all the analytic functions.
    private final List<Expr> commonPartitionExprs_;

    // map from analyticExprs_ to their corresponding analytic tuple slotrefs
    private final ExprSubstitutionMap analyticTupleSmap_;

    private AnalyticInfo(ArrayList<Expr> analyticExprs) {
        super(new ArrayList<Expr>(), new ArrayList<FunctionCallExpr>());
        analyticExprs_ = Expr.cloneList(analyticExprs);
        // Extract the analytic function calls for each analytic expr.
        for (Expr analyticExpr : analyticExprs) {
            aggregateExprs_.add(((AnalyticExpr) analyticExpr).getFnCall());
        }
        analyticTupleSmap_ = new ExprSubstitutionMap();
        commonPartitionExprs_ = computeCommonPartitionExprs();
    }

    /**
     * C'tor for cloning.
     */
    private AnalyticInfo(AnalyticInfo other) {
        super(other);
        analyticExprs_ =
                (other.analyticExprs_ != null) ? Expr.cloneList(other.analyticExprs_) : null;
        analyticTupleSmap_ = other.analyticTupleSmap_.clone();
        commonPartitionExprs_ = Expr.cloneList(other.commonPartitionExprs_);
    }

    public ExprSubstitutionMap getSmap() {
        return analyticTupleSmap_;
    }

    /**
     * Creates complete AnalyticInfo for analyticExprs, including tuple descriptors and
     * smaps.
     */
    public static AnalyticInfo create(
            ArrayList<Expr> analyticExprs, Analyzer analyzer) {
        Preconditions.checkState(analyticExprs != null && !analyticExprs.isEmpty());
        Expr.removeDuplicates(analyticExprs);
        AnalyticInfo result = new AnalyticInfo(analyticExprs);
        result.createTupleDescs(analyzer);

        // The tuple descriptors are logical. Their slots are remapped to physical tuples
        // during plan generation.
        result.outputTupleDesc_.setIsMaterialized(false);
        result.intermediateTupleDesc_.setIsMaterialized(false);

        // Populate analyticTupleSmap_
        Preconditions.checkState(analyticExprs.size() == result.outputTupleDesc_.getSlots().size());
        for (int i = 0; i < analyticExprs.size(); ++i) {
            result.analyticTupleSmap_.put(result.analyticExprs_.get(i),
                    new SlotRef(result.outputTupleDesc_.getSlots().get(i)));
            result.outputTupleDesc_.getSlots().get(i).setSourceExpr(result.analyticExprs_.get(i));
        }

        if (LOG.isDebugEnabled()) {
            LOG.debug("analytictuple=" + result.outputTupleDesc_.debugString());
            LOG.debug("analytictuplesmap=" + result.analyticTupleSmap_.debugString());
            LOG.debug("analytic info:\n" + result.debugString());
        }
        return result;
    }

    /**
     * Returns the intersection of the partition exprs of all the
     * analytic functions.
     */
    private List<Expr> computeCommonPartitionExprs() {
        List<Expr> result = Lists.newArrayList();
        for (Expr analyticExpr : analyticExprs_) {
            Preconditions.checkState(analyticExpr.isAnalyzed());
            List<Expr> partitionExprs = ((AnalyticExpr) analyticExpr).getPartitionExprs();
            if (partitionExprs == null) {
                continue;
            }
            if (result.isEmpty()) {
                result.addAll(partitionExprs);
            } else {
                result.retainAll(partitionExprs);
                if (result.isEmpty()) {
                    break;
                }
            }
        }
        return result;
    }

    @Override
    public String debugString() {
        StringBuilder out = new StringBuilder(super.debugString());
        out.append(MoreObjects.toStringHelper(this)
                .add("analytic_exprs", Expr.debugString(analyticExprs_))
                .add("smap", analyticTupleSmap_.debugString())
                .toString());
        return out.toString();
    }

    @Override
    protected String tupleDebugName() {
        return "analytic-tuple";
    }

    @Override
    public AnalyticInfo clone() {
        return new AnalyticInfo(this);
    }
}
