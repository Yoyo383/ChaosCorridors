#include "StateManager.hpp"


StateManager::StateManager() {
	running = true;
	shouldChangeState = false;
	nextState = nullptr;
}

StateManager::~StateManager() {
	while (!states.empty())
		popState();
}

void StateManager::pushState(std::unique_ptr<State> state) {
	states.push(std::move(state));
}

void StateManager::popState() {
	states.pop();
}

void StateManager::setNextState(std::unique_ptr<State> state) {
	nextState = std::move(state);
	shouldChangeState = true;
}

void StateManager::changeState() {
	if (shouldChangeState) {
		if (!states.empty())
			popState();
		pushState(std::move(nextState));
		shouldChangeState = false;
		nextState = nullptr;
	}
}

void StateManager::update() {
	states.top()->update();
}

void StateManager::draw() {
	states.top()->draw();
}

std::unique_ptr<State> StateManager::getCurrentState() {
	return std::move(states.top());
}

bool StateManager::isRunning() const {
	return running;
}

void StateManager::quit() {
	running = false;
}
