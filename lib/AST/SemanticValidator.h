/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#ifndef HERMES_AST_SEMANTICVALIDATOR_H
#define HERMES_AST_SEMANTICVALIDATOR_H

#include "hermes/AST/SemValidate.h"

#include "RecursiveVisitor.h"

namespace hermes {
namespace sem {

using namespace hermes::ESTree;

// Forward declarations
class FunctionContext;
class SemanticValidator;

//===----------------------------------------------------------------------===//
// Keywords

class Keywords {
 public:
  /// Identifier for "arguments".
  const UniqueString *const identArguments;
  /// Identifier for "eval".
  const UniqueString *const identEval;
  /// Identifier for "delete".
  const UniqueString *const identDelete;
  /// Identifier for "use strict".
  const UniqueString *const identUseStrict;

  Keywords(Context &astContext);
};

//===----------------------------------------------------------------------===//
// SemanticValidator

/// Class the performs all semantic validation
class SemanticValidator {
  friend class FunctionContext;

  Context &astContext_;
  /// A copy of Context::getSM() for easier access.
  SourceErrorManager &sm_;

  /// All semantic tables are persisted here.
  SemContext &semCtx_;

  /// Save the initial error count so we know whether we generated any errors.
  const unsigned initialErrorCount_;

  /// Keywords we will be checking for.
  Keywords kw_;

  /// The current function context.
  FunctionContext *funcCtx_{};

#ifndef NDEBUG
  /// Our parser detects strictness and initializes the flag in every node,
  /// but if we are reading an external AST, we must look for "use strict" and
  /// initialize the flag ourselves here.
  /// For consistency we always perform the detection, but in debug mode we also
  /// want to ensure that our results match what the parser generated. This
  /// flag indicates whether strictness is preset or not.
  bool strictnessIsPreset_{false};
#endif

 public:
  explicit SemanticValidator(Context &astContext, sem::SemContext &semCtx);

  // Perform the validation on whole AST.
  bool doIt(Node *rootNode);

  /// Perform the validation on an individual function.
  bool doFunction(Node *function, bool strict);

  /// Handle the default case for all nodes which we ignore, but we still want
  /// to visit their children.
  void visit(Node *node) {
    visitESTreeChildren(*this, node);
  }

  void visit(ProgramNode *node);
  void visit(FunctionDeclarationNode *funcDecl);
  void visit(FunctionExpressionNode *funcExpr);
  void visit(ArrowFunctionExpressionNode *arrowFunc);

  void visit(VariableDeclaratorNode *varDecl);

  void visit(IdentifierNode *identifier);

  void visit(ForInStatementNode *forIn);
  void visit(AssignmentExpressionNode *assignment);
  void visit(UpdateExpressionNode *update);

  void visit(LabeledStatementNode *labelStmt);

  void visit(RegExpLiteralNode *regexp);

  void visit(TryStatementNode *tryStatement);

  void visit(DoWhileStatementNode *loop);
  void visit(ForStatementNode *loop);
  void visit(WhileStatementNode *loop);
  void visit(SwitchStatementNode *switchStmt);

  void visit(BreakStatementNode *breakStmt);
  void visit(ContinueStatementNode *continueStmt);

  void visit(ReturnStatementNode *returnStmt);

  void visit(UnaryExpressionNode *unaryExpr);

 private:
  inline bool haveActiveContext() const {
    return funcCtx_ != nullptr;
  }

  inline FunctionContext *curFunction() {
    assert(funcCtx_ && "No active function context");
    return funcCtx_;
  }
  inline const FunctionContext *curFunction() const {
    assert(funcCtx_ && "No active function context");
    return funcCtx_;
  }

  /// Process a function declaration by creating a new FunctionContext. Update
  /// the context with the strictness of the function.
  /// \param node the current node
  /// \param id if not null, the associated name (for validation)
  /// \param params the parameter list
  /// \param body the body. It may be a BlockStatementNode, an EmptyNode (for
  ///     lazy functions), or an expression (for simple arrow functions).
  void visitFunction(
      FunctionLikeNode *node,
      const Node *id,
      NodeList &params,
      Node *body);

  /// Scan a list of directives in the beginning of a program of function
  /// (see ES5.1 4.1 - a directive is a statement consisting of a single
  /// string literal).
  /// Update the flags in the function context to reflect the directives. (We
  /// currently only recognize "use strict".)
  void scanDirectivePrologue(NodeList &body);

  /// Determine if the argument is something that can be assigned to: a
  /// variable or a property. 'arguments' cannot be assigned to in strict mode,
  /// but we don't support code generation for assigning to it in any mode.
  bool isLValue(const Node *node) const;

  /// In strict mode 'arguments' and 'eval' cannot be used in declarations.
  bool isValidDeclarationName(const Node *node) const;

  /// If the supplied Identifier node is not a valid name to be used in a
  /// declaration, report an error.
  void validateDeclarationName(const Node *node);

  /// A debugging method to set the strictness of a function-like node to
  /// the curent strictness, asserting that it doesn't change if it had been
  /// preset.
  void updateNodeStrictness(FunctionLikeNode *node);

  /// Get the LabelDecorationBase depending on the node type.
  static LabelDecorationBase *getLabelDecorationBase(StatementNode *node);
};

//===----------------------------------------------------------------------===//
// FunctionContext

/// Holds all per-function state, specifically label tables. Should always be
/// constructed on the stack.
class FunctionContext {
  SemanticValidator *validator_;
  FunctionContext *oldContextValue_;

 public:
  struct Label {
    /// Where it was declared.
    IdentifierNode *declarationNode;

    /// Statement targeted by the label. It is either a LoopStatement or a
    /// LabeledStatement.
    StatementNode *targetStatement;
  };

  /// The associated seminfo object
  sem::FunctionInfo *const semInfo;

  /// The most nested active try statement.
  TryStatementNode *activeTry = nullptr;
  /// The most nested active loop statement.
  LoopStatementNode *activeLoop = nullptr;
  /// The most nested active loop or switch statement.
  StatementNode *activeSwitchOrLoop = nullptr;
  /// Is this function in strict mode.
  bool strictMode = false;

  /// The currently active labels in the function.
  llvm::DenseMap<NodeLabel, Label> labelMap;

  explicit FunctionContext(
      SemanticValidator *validator,
      bool strictMode,
      FunctionLikeNode *node);

  ~FunctionContext();

  /// \return true if this is the "global scope" function context, in other
  /// words not a real function.
  bool isGlobalScope() const {
    return !oldContextValue_;
  }

  /// Allocate a new label in the current context.
  unsigned allocateLabel() {
    return semInfo->allocateLabel(activeTry);
  }
};

} // namespace sem
} // namespace hermes

#endif // HERMES_AST_SEMANTICVALIDATOR_H