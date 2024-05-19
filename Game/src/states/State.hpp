#pragma once

class StateManager;

class State {
public:
	virtual void update() = 0;
	virtual void draw() = 0;
};

