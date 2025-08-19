#include "scanner/scanner_context.h"

namespace chtl {

ScannerContext::ScannerContext() : currentLine_(1), currentColumn_(1) {
    stateStack_.push(ScannerState::NORMAL);
}

void ScannerContext::pushState(ScannerState state) {
    stateStack_.push(state);
}

void ScannerContext::popState() {
    if (!stateStack_.empty()) {
        stateStack_.pop();
    }
}

ScannerState ScannerContext::getCurrentState() const {
    if (stateStack_.empty()) {
        return ScannerState::NORMAL;
    }
    return stateStack_.top();
}

bool ScannerContext::isInState(ScannerState state) const {
    std::stack<ScannerState> temp = stateStack_;
    while (!temp.empty()) {
        if (temp.top() == state) {
            return true;
        }
        temp.pop();
    }
    return false;
}

void ScannerContext::updatePosition(char ch) {
    if (ch == '\n') {
        currentLine_++;
        currentColumn_ = 1;
    } else {
        currentColumn_++;
    }
}

void ScannerContext::reset() {
    while (!stateStack_.empty()) {
        stateStack_.pop();
    }
    stateStack_.push(ScannerState::NORMAL);
    currentLine_ = 1;
    currentColumn_ = 1;
}

} // namespace chtl