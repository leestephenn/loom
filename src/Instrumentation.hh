//! @file Instrumentation.h  Declaration of @ref loom::Instrumentation.
/*
 * Copyright (c) 2012-2013,2015-2016 Jonathan Anderson
 * Copyright (c) 2016 Cem Kilic
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme, by BAE Systems,
 * the University of Cambridge Computer Laboratory, and Memorial University
 * under DARPA/AFRL contract FA8650-15-C-7558 ("CADETS"), as part of the
 * DARPA Transparent Computing (TC) research program, and at Memorial University
 * under the NSERC Discovery program (RGPIN-2015-06048).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef  LOOM_INSTRUMENTATION_FN_H
#define  LOOM_INSTRUMENTATION_FN_H

#include "InstrStrategy.hh"
#include "IRUtils.hh"
#include "PolicyFile.hh"

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>

namespace llvm {
  class Instruction;
  class LLVMContext;
  class Module;
}

namespace loom {

/**
 * Instrumentation code (which could be a function or a set of inline blocks)
 * that receives program events such as "called foo(42,97)" and takes
 * policy-defined actions (e.g., logs events or updates counters).
 *
 * @invariant Instrumentation must have a preamble block, which contains
 *            instructions to execute first for all instrumented events
 *            (e.g., logging calls) and an "exit" block after all actions
 *            required by the instrumentation have been taken.
 *            New instrumentation actions are added in BasicBlocks in between.
 */
class Instrumentation {
public:
  Instrumentation(llvm::SmallVector<llvm::Value*, 4> Values,
                  llvm::BasicBlock *Preamble, llvm::BasicBlock *End)
    : InstrValues(std::move(Values)), Preamble(Preamble), End(End)
  {
  }

  //! Retrieve the values passed into the instrumentation.
  llvm::ArrayRef<llvm::Value*> Values() { return InstrValues; }

  /**
   * Get an IRBuilder that can be used to insert new instructions into the
   * instrumentation's preamble block.
   *
   * The preamble is a block where we can put instructions that should run
   * whenever the instrumentation is called, regardless of the values that
   * are passed into it. This could be used to implement gprof-style counting
   * or simple logging.
   */
  llvm::IRBuilder<> GetPreambleBuilder();

  /// Add a new link in the chain of actions for this instrumentation.
  llvm::IRBuilder<> AddAction(llvm::StringRef Name);


private:
  llvm::SmallVector<llvm::Value*, 4> InstrValues;  //!< Instrumented values
  llvm::BasicBlock *Preamble;   //!< Cross-instrumentation logging, etc.
  llvm::BasicBlock *End;        //!< End of the instrumentation chain.
};

}

#endif  /* !LOOM_INSTRUMENTATION_FN_H */