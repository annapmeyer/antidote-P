#ifndef ASTNODE_H
#define ASTNODE_H

#include<vector>
using namespace std;

/**
 * The AST class hierarchy here looks like the following:
 * ASTNode (abstract)
 *   SequenceNode
 *   ITENode (no constructor)
 *      ITEImpurityNode
 *      ITEModelsNode
 *   BestSplitNode
 *   FilterNode
 *   SummaryNode
 *   ReturnNode
 *
 * It represents this simple Decision-Tree-Learning-Classification DSL:
 *        Statements S := S_1; S_2; ...; S_n
 *                      | if impurity(T) = 0 then S_1 else S_2
 *                      | if x models phi then S_1 else S_2
 *                      | phi <- bestsplit(T)
 *                      | T <- filter(T, phi)
 *                      | T <- filter(T, not phi)
 *                      | p <- summary(T)
 * Return Statements R := return p
 *           Program P := S; R
 * Note that x, T, phi, and p are not non-terminals---they are part of the syntax.
 * Indeed, x is fixed a priori, and the semantics of the program update a simple state
 * consisting of the (T, phi, p) triple.
 *
 * Additionally, this file includes an interface for AST traversal (Visitor).
 **/


class Visitor;


class ASTNode {
public:
    static ASTNode* buildTree(int depth);

    virtual ~ASTNode() = 0;
    virtual void accept(Visitor &v) const = 0;
};


class SequenceNode : public ASTNode {
private:
    vector<ASTNode*> children;

public:
    SequenceNode(const vector<ASTNode*> &children);
    ~SequenceNode();
    void accept(Visitor &v) const;

    const vector<ASTNode*>& get_children() const { return children; }
};


class ITENode : public ASTNode {
protected:
    const ASTNode *then_child, *else_child;

public:
    ITENode(const ASTNode *then_child, const ASTNode *else_child);
    ~ITENode();
    const ASTNode* get_then_child() const { return then_child; }
    const ASTNode* get_else_child() const { return else_child; }
};


class ITEImpurityNode : public ITENode {
public:
    ITEImpurityNode(const ASTNode *then_child, const ASTNode *else_child)
        : ITENode(then_child, else_child) {};
    void accept(Visitor &v) const;
};


class ITEModelsNode : public ITENode {
public:
    ITEModelsNode(const ASTNode *then_child, const ASTNode *else_child)
        : ITENode(then_child, else_child) {};
    void accept(Visitor &v) const;
};


class BestSplitNode : public ASTNode {
public:
    BestSplitNode() {};
    ~BestSplitNode() {};
    void accept(Visitor &v) const;
};


class FilterNode : public ASTNode {
private:
    bool mode; // Determines whether or not phi is negated

public:
    FilterNode(bool mode);
    ~FilterNode() {};
    void accept(Visitor &v) const;

    bool get_mode() const { return mode; }
};


class SummaryNode : public ASTNode {
public:
    SummaryNode() {};
    ~SummaryNode() {};
    void accept(Visitor &v) const;
};


class ReturnNode : public ASTNode {
public:
    ReturnNode() {};
    ~ReturnNode() {};
    void accept(Visitor &v) const;
};


class Visitor {
public:
    virtual void visit(const SequenceNode &node) = 0;
    virtual void visit(const ITEImpurityNode &node) = 0;
    virtual void visit(const ITEModelsNode &node) = 0;
    virtual void visit(const BestSplitNode &node) = 0;
    virtual void visit(const FilterNode &node) = 0;
    virtual void visit(const SummaryNode &node) = 0;
    virtual void visit(const ReturnNode &node) = 0;
};


#endif
