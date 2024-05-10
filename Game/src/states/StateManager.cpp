#include "StateManager.hpp"

StateManager::StateManager() {
	running = true;
	shouldChangeState = false;
	shouldRemoveState = false;
	nextState = nullptr;
}

StateManager::~StateManager() {
	while (!states.empty())
		removeState();
}

void StateManager::addState(std::unique_ptr<State> state) {
	nextState = std::move(state);
	shouldChangeState = true;
	shouldRemoveState = false;
}

void StateManager::removeState() {
	states.pop();
}

void StateManager::setState(std::unique_ptr<State> state) {
	nextState = std::move(state);
	shouldChangeState = true;
	shouldRemoveState = true;
}

void StateManager::changeState() {
	if (shouldChangeState) {
		if (!states.empty() && shouldRemoveState)
			removeState();
		states.push(std::move(nextState));
		shouldChangeState = false;
		nextState = nullptr;
	}
}

void StateManager::update() {
	if (running)
		states.top()->update();
}

void StateManager::draw() {
	if (running)
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
