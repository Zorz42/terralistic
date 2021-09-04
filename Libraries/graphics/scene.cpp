#include "graphics-internal.hpp"

static bool running_scene = true, disable_events_gl;
bool key_states[(int)gfx::Key::UNKNOWN];

void gfx::Scene::onKeyDownCallback(Key key_) {
    if(!key_states[(int)key_]) {
        key_states[(int)key_] = true;
        if(_can_receive_events)
            onKeyDown(key_);
        for(GraphicalModule* module : modules)
            if(module->_can_receive_events)
                module->onKeyDown(key_);
    }
}

void gfx::Scene::enableAllEvents(bool enable) {
    _can_receive_events = enable;
    for(GraphicalModule* module : modules)
        module->_can_receive_events = enable;
}

bool gfx::GraphicalModule::getKeyState(Key key_) {
    return _can_receive_events && key_states[(int)key_];
}

gfx::Key translateMouseKey(sf::Mouse::Button sfml_button) {
    switch(sfml_button) {
        case sf::Mouse::Left: return gfx::Key::MOUSE_LEFT;
        case sf::Mouse::Middle: return gfx::Key::MOUSE_MIDDLE;
        case sf::Mouse::Right: return gfx::Key::MOUSE_RIGHT;
        default: return gfx::Key::UNKNOWN;
    }
}

gfx::Key translateKeyboardKey(sf::Keyboard::Key sfml_button) {
    switch(sfml_button) {
        case sf::Keyboard::Key::A: return gfx::Key::A;
        case sf::Keyboard::Key::B: return gfx::Key::B;
        case sf::Keyboard::Key::C: return gfx::Key::C;
        case sf::Keyboard::Key::D: return gfx::Key::D;
        case sf::Keyboard::Key::E: return gfx::Key::E;
        case sf::Keyboard::Key::F: return gfx::Key::F;
        case sf::Keyboard::Key::G: return gfx::Key::G;
        case sf::Keyboard::Key::H: return gfx::Key::H;
        case sf::Keyboard::Key::I: return gfx::Key::I;
        case sf::Keyboard::Key::J: return gfx::Key::J;
        case sf::Keyboard::Key::K: return gfx::Key::K;
        case sf::Keyboard::Key::L: return gfx::Key::L;
        case sf::Keyboard::Key::M: return gfx::Key::M;
        case sf::Keyboard::Key::N: return gfx::Key::N;
        case sf::Keyboard::Key::O: return gfx::Key::O;
        case sf::Keyboard::Key::P: return gfx::Key::P;
        case sf::Keyboard::Key::Q: return gfx::Key::Q;
        case sf::Keyboard::Key::R: return gfx::Key::R;
        case sf::Keyboard::Key::S: return gfx::Key::S;
        case sf::Keyboard::Key::T: return gfx::Key::T;
        case sf::Keyboard::Key::U: return gfx::Key::U;
        case sf::Keyboard::Key::V: return gfx::Key::V;
        case sf::Keyboard::Key::W: return gfx::Key::W;
        case sf::Keyboard::Key::X: return gfx::Key::X;
        case sf::Keyboard::Key::Y: return gfx::Key::Y;
        case sf::Keyboard::Key::Z: return gfx::Key::Z;
        case sf::Keyboard::Key::Num0: return gfx::Key::NUM0;
        case sf::Keyboard::Key::Num1: return gfx::Key::NUM1;
        case sf::Keyboard::Key::Num2: return gfx::Key::NUM2;
        case sf::Keyboard::Key::Num3: return gfx::Key::NUM3;
        case sf::Keyboard::Key::Num4: return gfx::Key::NUM4;
        case sf::Keyboard::Key::Num5: return gfx::Key::NUM5;
        case sf::Keyboard::Key::Num6: return gfx::Key::NUM6;
        case sf::Keyboard::Key::Num7: return gfx::Key::NUM7;
        case sf::Keyboard::Key::Num8: return gfx::Key::NUM8;
        case sf::Keyboard::Key::Num9: return gfx::Key::NUM9;
        case sf::Keyboard::Key::Space: return gfx::Key::SPACE;
        case sf::Keyboard::Key::Escape: return gfx::Key::ESCAPE;
        case sf::Keyboard::Key::Enter: return gfx::Key::ENTER;
        case sf::Keyboard::Key::LShift:
        case sf::Keyboard::Key::RShift: return gfx::Key::SHIFT;
        case sf::Keyboard::Key::Backspace: return gfx::Key::BACKSPACE;
        case sf::Keyboard::Key::LControl: case sf::Keyboard::Key::RControl: return gfx::Key::CTRL;
        default: return gfx::Key::UNKNOWN;
    }
}

void gfx::returnFromScene() {
    running_scene = false;
}

void gfx::Scene::_operateEvent(sf::Event event) {
    if (event.type == sf::Event::Resized)
        setWindowSize(event.size.width / global_scale, event.size.height / global_scale);
    else if (event.type == sf::Event::MouseButtonPressed) {
        gfx::Key key = translateMouseKey(event.mouseButton.button);
        bool clicked_text_box = false;
        if (key == Key::MOUSE_LEFT) {
            if (!disable_events_gl || disable_events)
                for (TextInput* i : text_inputs) {
                    i->active = i->isHovered(mouse_x, mouse_y);
                    if (i->active)
                        clicked_text_box = true;
                }
    
            for (GraphicalModule* module : modules)
                if (!disable_events_gl || module->disable_events)
                    for (TextInput* i : module->text_inputs) {
                        i->active = i->isHovered(mouse_x, mouse_y);
                        if (i->active)
                            clicked_text_box = true;
                    }
        }
        if (key != Key::UNKNOWN && !clicked_text_box)
            onKeyDownCallback(key);
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        gfx::Key key = translateMouseKey(event.mouseButton.button);
        if (key != Key::UNKNOWN)
            key_states[(int)key] = false;
    }
    else if (event.type == sf::Event::KeyPressed) {
        gfx::Key key = translateKeyboardKey(event.key.code);
        if (key == Key::BACKSPACE) {
            for (TextInput* i : text_inputs)
                if (i->active && !i->getText().empty()) {
                    std::string str = i->getText();
                    str.pop_back();
                    i->setText(str);
                }
            for (GraphicalModule* module : modules)
                for (TextInput* i : module->text_inputs)
                    if (i->active && !i->getText().empty()) {
                        std::string str = i->getText();
                        str.pop_back();
                        i->setText(str);
                    }
        }
        if (key != Key::UNKNOWN)
            onKeyDownCallback(key);
    }
    else if (event.type == sf::Event::KeyReleased) {
        gfx::Key key = translateKeyboardKey(event.key.code);
        if (key != Key::UNKNOWN)
            key_states[(int)key] = false;
    }
    else if (event.type == sf::Event::TextEntered) {
        char c = event.text.unicode;
        if(c == '\b')
            return;
    
        for (TextInput* i : text_inputs)
            if (i->active) {
                char result = c;
                if (!i->ignore_one_input) {
                    if (i->textProcessing)
                        result = i->textProcessing(result, (int)i->getText().size());
                    if (result)
                        i->setText(i->getText() + result);
                }
                i->ignore_one_input = false;
            }
        for (GraphicalModule* module : modules)
            for (TextInput* i : module->text_inputs)
                if (i->active) {
                    char result = c;
                    if (!i->ignore_one_input) {
                        if (i->textProcessing)
                            result = i->textProcessing(result, (int)i->getText().size());
                        if (result)
                            i->setText(i->getText() + result);
                    }
                    i->ignore_one_input = false;
                }
    }
    else if (event.type == sf::Event::MouseWheelScrolled)
        onMouseScroll(event.mouseWheelScroll.delta);
    else if (event.type == sf::Event::Closed)
        window->close();
}

void gfx::Scene::run() {
    init();
    for (GraphicalModule* module : modules)
        module->init();
    
    while(running_scene && window->isOpen()) {
        unsigned int start = getTicks();
        
        disable_events_gl = disable_events;
        for(GraphicalModule* module : modules) {
            if(disable_events_gl)
                break;
            disable_events_gl = module->disable_events;
        }
        
        _can_receive_events = !disable_events_gl || disable_events;
        for(GraphicalModule* module : modules)
            module->_can_receive_events = !disable_events_gl || module->disable_events;
        
        mouse_x = sf::Mouse::getPosition(*window).x / global_scale;
        mouse_y = sf::Mouse::getPosition(*window).y / global_scale;
        for(GraphicalModule* module : modules) {
            module->mouse_x = mouse_x;
            module->mouse_y = mouse_y;
        }
        
        sf::Event event;
        while(window->pollEvent(event))
            _operateEvent(event);
        
        update();
        for(GraphicalModule* module : modules)
            module->update();
        
        clearWindow();
        
        render();
        for(GraphicalModule* module : modules)
            module->render();
        
        updateWindow();
        
        frame_length = getTicks() - start;
    }
    
    running_scene = true;
    
    stop();
    for(GraphicalModule* module : modules) {
        module->stop();
        delete module;
    }
}