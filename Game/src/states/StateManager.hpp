#pragma once
#include <stack>
#include "State.hpp"
#include <memory>

class StateManager {
public:
	StateManager();
	~StateManager();

	void addState(std::unique_ptr<State> state);
	void removeState();
	void setState(std::unique_ptr<State> state);
	void changeState();

	void update();
	void draw();

	std::unique_ptr<State> getCurrentState();

	bool isRunning() const;
	void quit();

private:
	std::stack<std::unique_ptr<State>> states;
	bool running;
	bool shouldChangeState;
	bool shouldRemoveState;
	std::unique_ptr<State> nextState;
};
