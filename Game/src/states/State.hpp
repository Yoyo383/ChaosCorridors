#pragma once
#include "SFML/Graphics.hpp"
#include "../graphics.hpp"

class StateManager;

class State {
public:
	virtual void update() = 0;
	virtual void draw() = 0;
};

