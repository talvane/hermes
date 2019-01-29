/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the LICENSE
 * file in the root directory of this source tree.
 */
#include "hermes/AST/SemValidate.h"

#include "hermes/Support/PerfSection.h"

#include "SemanticValidator.h"

using namespace hermes::ESTree;

namespace hermes {
namespace sem {

bool validateAST(Context &astContext, SemContext &semCtx, Node *root) {
  PerfSection validation("Validating JavaScript function AST");
  // Validate the entire AST.
  SemanticValidator validator{astContext, semCtx};
  return validator.doIt(root);
}

bool validateFunctionAST(
    Context &astContext,
    SemContext &semCtx,
    Node *function,
    bool strict) {
  PerfSection validation("Validating JavaScript function AST: Deep");
  SemanticValidator validator{astContext, semCtx};
  return validator.doFunction(function, strict);
}

} // namespace sem
} // namespace hermes