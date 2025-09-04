#include <memory>

#include "gpioListener.hxx"

gpioListener::gpioListener(gpiod::line::offset line_in, std::optional<gpiod::line::offset> line_out): 
    line_in(line_in), line_out(line_out) {
    auto chip = gpiod::chip("dev/gpiochip0");
    request_in = std::make_unique<gpiod::line_request>(chip.prepare_request()
        .set_consumer("IN consumer")
        .add_line_settings(
            line_in,
            ::gpiod::line_settings()
                .set_direction(::gpiod::line::direction::INPUT)
                .set_edge_detection(::gpiod::line::edge::BOTH)
                .set_bias(::gpiod::line::bias::PULL_UP)
                .set_debounce_period(debounce)
        )
    .do_request());

    if (line_out.has_value()) {
        request_out = chip.prepare_request()
            .set_consumer("OUT consumer")
            .add_line_settings(
                line_out.value(),
                gpiod::line_settings()
                    .set_direction(gpiod::line::direction::OUTPUT)
            )
        .do_request();
    } else {
        // default dummy req
        request_out = std::nullopt;
    }
};


void gpioListener::_run() {
    ::gpiod::edge_event_buffer buffer(1);
    for (;;) {
        request_in->read_edge_events(buffer);

        for (const auto &event : buffer) {
            switch (event.type()) {
                case ::gpiod::edge_event::event_type::RISING_EDGE:
                    active = true;
                    if (line_out.has_value()) 
                        request_out->set_value(line_out.value(), ::gpiod::line::value::INACTIVE);
                    cv.notify_one();
                break;

                case ::gpiod::edge_event::event_type::FALLING_EDGE:
                    active = false;
                    if (line_out.has_value())
                        request_out->set_value(line_out.value(), ::gpiod::line::value::ACTIVE);
                    cv.notify_one();
                break;
            }
        }
    }
}

void gpioListener::start() {
    _run();
};
