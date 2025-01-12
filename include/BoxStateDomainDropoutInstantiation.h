#ifndef BOXSTATEDOMAINDROPOUTINSTANTIATION_H
#define BOXSTATEDOMAINDROPOUTINSTANTIATION_H

#include "BoxStateDomainTemplate.hpp"
#include "CategoricalDistribution.hpp"
#include "DataReferences.h"
#include "Feature.hpp"
#include "Interval.h"
#include "SymbolicPredicate.hpp"
#include <list>
#include <optional>
#include <utility>
#include <vector>


/**
 * We have the individual types for each component of the state tuple:
 *   - Training set abstraction: we will consider a fixed training set
 *     with up to n-many elements missing
 *   - Predicate abstraction: we will consider a finite set of (symbolic) predicates,
 *     each individually similar to the Predicate.hpp Predicate class
 *   - Posterior distribution abstraction: we will have a categorical
 *     probability density function where each value is an interval
 */


class TrainingReferencesWithDropout {
public:
    struct DropoutCounts {
        std::vector<int> counts;
        int num_dropout;
        int num_add;
        std::pair<int, int> add_sens_info;
        int num_labels_flip;
        std::pair<int, int> label_sens_info;
        int num_features_flip;
        int feature_flip_index;
        float feature_flip_amt;
    };

    DataReferences training_references;
    int num_dropout;
    int num_add;
    std::pair<int, int> add_sens_info;
    int num_labels_flip;
    std::pair<int, int> label_sens_info;
    int num_features_flip;
    int feature_flip_index;
    float feature_flip_amt;

    TrainingReferencesWithDropout() {} // Constructed like this should be a bottom element
    TrainingReferencesWithDropout(DataReferences training_references, int num_dropout, int num_add, std::pair<int, int> add_sens_info, int num_labels_flip, std::pair<int, int> label_sens_info, int num_features_flip, int feature_flip_index, float feature_flip_amt);

    std::vector<int> baseCounts() const;
    std::pair<DropoutCounts, DropoutCounts> splitCounts(const SymbolicPredicate &phi) const;
    TrainingReferencesWithDropout pureSetRestriction(std::list<int> pure_possible_classes) const;
    TrainingReferencesWithDropout filter(const SymbolicPredicate &phi, bool positive_flag) const; // Returns a new object
};


typedef std::vector<std::optional<SymbolicPredicate>> PredicateAbstraction;

typedef CategoricalDistribution<Interval<double>> PosteriorDistributionAbstraction;


/**
 * Next, we have the individual consituent domains
 */


class TrainingSetDropoutDomain : public TrainingSetDomainTemplate<TrainingReferencesWithDropout> {
public:
    TrainingReferencesWithDropout meetImpurityEqualsZero(const TrainingReferencesWithDropout &element) const;
    TrainingReferencesWithDropout meetImpurityNotEqualsZero(const TrainingReferencesWithDropout &element) const;

    bool isBottomElement(const TrainingReferencesWithDropout &element) const;
    TrainingReferencesWithDropout binary_join(const TrainingReferencesWithDropout &e1, const TrainingReferencesWithDropout &e2) const;
};


class PredicateSetDomain : public PredicateDomainTemplate<PredicateAbstraction> {
public:
    PredicateAbstraction meetPhiIsBottom(const PredicateAbstraction &element) const;
    PredicateAbstraction meetPhiIsNotBottom(const PredicateAbstraction &element) const;
    PredicateAbstraction meetXModelsPhi(const PredicateAbstraction &element, const FeatureVector &x) const;
    PredicateAbstraction meetXNotModelsPhi(const PredicateAbstraction &element, const FeatureVector &x) const;

    bool isBottomElement(const PredicateAbstraction &element) const;
    PredicateAbstraction binary_join(const PredicateAbstraction &e1, const PredicateAbstraction &e2) const;
};


class PosteriorDistributionIntervalDomain : public PosteriorDistributionDomainTemplate<PosteriorDistributionAbstraction> {
public:
    bool isBottomElement(const PosteriorDistributionAbstraction &element) const;
    PosteriorDistributionAbstraction binary_join(const PosteriorDistributionAbstraction &e1, const PosteriorDistributionAbstraction &e2) const;
};


/**
 * Finally, the remaining box domain subclass
 */


class BoxDropoutDomain : public BoxStateDomainTemplate<TrainingReferencesWithDropout, PredicateAbstraction, PosteriorDistributionAbstraction> {
private:
    typedef std::pair<SymbolicPredicate, Interval<double>> ScoreEntry;
    void computePredicatesAndScores(
            std::list<ScoreEntry> &exists_nontrivial,
            std::list<const ScoreEntry *> &forall_nontrivial,
            const TrainingReferencesWithDropout &training_set_abstraction,
            int feature_index ) const;
    void computeBooleanFeaturePredicateAndScore(
            std::list<ScoreEntry> &exists_nontrivial,
            std::list<const ScoreEntry *> &forall_nontrivial,
            const TrainingReferencesWithDropout &training_set_abstraction,
            int feature_index ) const;
    void computeNumericFeaturePredicatesAndScores(
            std::list<ScoreEntry> &exists_nontrivial,
            std::list<const ScoreEntry *> &forall_nontrivial,
            const TrainingReferencesWithDropout &training_set_abstraction,
            int feature_index ) const;
    /*void updateSplitCountsDropout(
        std::pair<TrainingReferencesWithDropout::DropoutCounts, TrainingReferencesWithDropout::DropoutCounts> &split_counts
    ) const;
    void addPhiAndPushback(
        SymbolicPredicate &phi,
         std::list<ScoreEntry> &exists_nontrivial, 
         std::list<const ScoreEntry *> &forall_nontrivial, 
         std::pair<TrainingReferencesWithDropout::DropoutCounts, TrainingReferencesWithDropout::DropoutCounts> &split_counts
    ) const;*/

public:
    using BoxStateDomainTemplate::BoxStateDomainTemplate; // Inherit the constructor

    PredicateAbstraction bestSplit(const TrainingReferencesWithDropout &training_set_abstraction) const;
    TrainingReferencesWithDropout filter(const TrainingReferencesWithDropout &training_set_abstraction, const PredicateAbstraction &predicate_abstraction) const;
    TrainingReferencesWithDropout filterNegated(const TrainingReferencesWithDropout &training_set_abstraction, const PredicateAbstraction &predicate_abstraction) const;
    PosteriorDistributionAbstraction summary(const TrainingReferencesWithDropout &training_set_abstraction) const;
};


#endif
