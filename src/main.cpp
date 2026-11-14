#include <iostream>
#include <boost/asio.hpp>
#include <chrono>

namespace asio = boost::asio;
using namespace std::chrono_literals;

enum class KettleMode
{
    Idle,
    Heating,
    KeepingWarm,
    Error
};

class Kettle
{
private:
    KettleMode mode = KettleMode::Idle;
    asio::steady_timer timer_;

    int temperature = 20;
    int final_temperature = 100;

    static constexpr int step_degrees = 5;
    static constexpr std::chrono::milliseconds heat_interval{500};
    static constexpr std::chrono::milliseconds keep_warm_interval{10000};

    void onTick(const boost::system::error_code& ec)
    {
        if (ec == asio::error::operation_aborted)
            return;

        if (ec)
        {
            mode = KettleMode::Error;
            return;
        }

        if (mode != KettleMode::Heating)
            return;

        temperature += step_degrees;

        if (temperature > final_temperature)
            temperature = final_temperature;

        std::cout << "[Kettle] Heating - " << temperature << " C\n";

        if (temperature >= final_temperature)
        {
            mode = KettleMode::KeepingWarm;
            std::cout << "[Kettle] Keeping warm for 10s\n";

            timer_.expires_after(keep_warm_interval);
            timer_.async_wait([this](const boost::system::error_code& ec)
            {
                if (ec == asio::error::operation_aborted)
                    return;

                if (ec)
                {
                    mode = KettleMode::Error;
                    return;
                }

                mode = KettleMode::Idle;
                std::cout << "[Kettle] Idle\n";
            });

            return;
        }

        timer_.expires_after(heat_interval);
        timer_.async_wait([this](const boost::system::error_code& ec)
        {
            onTick(ec);
        });
    }

public:
    explicit Kettle(asio::io_context& io) : timer_(io)
    {
        std::cout << "[Create Kettle]\n";
    }

    void start()
    {
        mode = KettleMode::Heating;
        std::cout << "[Start Kettle] Temperature - " << temperature << " C\n";

        timer_.expires_after(heat_interval);
        timer_.async_wait([this](const boost::system::error_code& ec)
        {
            onTick(ec);
        });
    }
    ~Kettle()
    {
        boost::system::error_code ec;
        timer_.cancel(ec);
        std::cout << "[Delete Kettle]\n";
    }
};

int main()
{
    asio::io_context ctx;
    Kettle test(ctx);
    test.start();

    ctx.run();
}