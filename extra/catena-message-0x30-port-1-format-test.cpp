/*

Name:   catena-message-0x30-port-1-format-test.cpp

Function:
        Generate test vectors for port 0x01 format 0x30 messages.

Copyright and License:
        See accompanying LICENSE file

Author:
        Pranau R, MCCI Corporation   June 2023

*/

#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

std::string key;
std::string value;

template <typename T>
struct val
    {
    bool fValid;
    T v;
    };

// Touch Channel Data
struct touchData
    {
    int16_t ch1;
    int16_t ch2;
    int16_t amplitude;
    };

// Touch Counter
struct counter
    {
    int16_t touchCountLeft;
    int16_t touchCountRight;
    };

struct Measurements
    {
    val<float> Vbat;
    val<float> Vsys;
    val<float> Vbus;
    val<std::uint8_t> Boot;
    val<touchData> TouchData;
    val<counter> TouchCount;
    };

std::uint16_t encode16s(float v)
    {
    float nv = std::floor(v + 0.5f);

    if (nv > 32767.0f)
        return 0x7FFFu;
    else if (nv < -32768.0f)
        return 0x8000u;
    else
        {
        return (std::uint16_t) std::int16_t(nv);
        }
    }

std::uint16_t encode16u(float v)
    {
    float nv = std::floor(v + 0.5f);
    if (nv > 65535.0f)
        return 0xFFFFu;
    else if (nv < 0.0f)
        return 0;
    else
        {
        return std::uint16_t(nv);
        }
    }

std::uint16_t encodeV(float v)
    {
    return encode16s(v * 4096.0f);
    }

std::uint16_t encodeChannel(int16_t channel)
    {
    return encode16u(channel);
    }

std::int16_t encodeAmplitude(int16_t amplitude)
    {
    return encode16s(amplitude);
    }

std::uint16_t encodeTouch(int16_t touch)
    {
    return encode16s(touch);
    }

class Buffer : public std::vector<std::uint8_t>
    {
public:
    Buffer() : std::vector<std::uint8_t>() {};

    void push_back_be(std::uint16_t v)
        {
        this->push_back(std::uint8_t(v >> 8));
        this->push_back(std::uint8_t(v & 0xFF));
        }
    };

void encodeMeasurement(Buffer &buf, Measurements &m)
    {
    std::uint8_t flags = 0;

    // sent the type byte
    buf.clear();
    buf.push_back(0x30);
    buf.push_back(0u); // flag byte.

    // put the fields
    if (m.Vbat.fValid)
        {
        flags |= 1 << 0;
        buf.push_back_be(encodeV(m.Vbat.v));
        }

    if (m.Vbus.fValid)
        {
        flags |= 1 << 1;
        buf.push_back_be(encodeV(m.Vbus.v));
        }

    if (m.Boot.fValid)
        {
        flags |= 1 << 2;
        buf.push_back(m.Boot.v);
        }

    if (m.TouchData.fValid)
        {
        flags |= 1 << 3;

        buf.push_back_be(encodeChannel(m.TouchData.v.ch1));
        buf.push_back_be(encodeChannel(m.TouchData.v.ch2));
        buf.push_back_be(encodeAmplitude(m.TouchData.v.amplitude));
        }

    if (m.TouchCount.fValid)
        {
        flags |= 1 << 4;

        buf.push_back_be(encodeTouch(m.TouchCount.v.touchCountLeft));
        buf.push_back_be(encodeTouch(m.TouchCount.v.touchCountRight));
        }

    // update the flags
    buf.data()[1] = flags;
    }

void logMeasurement(Measurements &m)
    {
    class Padder {
    public:
        Padder() : m_first(true) {}
        const char *get() {
            if (this->m_first)
                {
                this->m_first = false;
                return "";
                }
            else
                return " ";
            }
        const char *nl() {
            return this->m_first ? "" : "\n";
            }
    private:
        bool m_first;
    } pad;

    // put the fields
    if (m.Vbat.fValid)
        {
        std::cout << pad.get() << "Vbat " << m.Vbat.v;
        }

    if (m.Vbus.fValid)
        {
        std::cout << pad.get() << "Vbus " << m.Vbus.v;
        }

    if (m.Boot.fValid)
        {
        std::cout << pad.get() << "Boot " << unsigned(m.Boot.v);
        }

    if (m.TouchData.fValid)
        {
        std::cout << pad.get() << "Channel1 " << m.TouchData.v.ch1;
        std::cout << pad.get() << "Channel2 " << m.TouchData.v.ch2;
        std::cout << pad.get() << "Amplitude " << m.TouchData.v.amplitude;
        }

    if (m.TouchCount.fValid)
        {
        std::cout << pad.get() << "LeftTouchCounter " << m.TouchCount.v.touchCountLeft;
        std::cout << pad.get() << "RightTouchCounter " << m.TouchCount.v.touchCountRight;
        }

    // make the syntax cut/pastable.
    std::cout << pad.get() << ".\n";
    }

void putTestVector(Measurements &m)
    {
    Buffer buf {};
    logMeasurement(m);
    encodeMeasurement(buf, m);
    bool fFirst;

    fFirst = true;
    for (auto v : buf)
        {
        if (! fFirst)
            std::cout << " ";
        fFirst = false;
        std::cout.width(2);
        std::cout.fill('0');
        std::cout << std::hex << unsigned(v);
        }
    std::cout << "\n";
    }

int main(int argc, char **argv)
    {
    Measurements m {0};
    Measurements m0 {0};
    bool fAny;

    std::cout << "Input one or more lines of name/value tuples, ended by '.'\n";

    fAny = false;
    while (std::cin.good())
        {
        bool fUpdate = true;
        key.clear();

        std::cin >> key;

        if (key == "Vbat")
            {
            std::cin >> m.Vbat.v;
            m.Vbat.fValid = true;
            }
        else if (key == "Vbus")
            {
            std::cin >> m.Vbus.v;
            m.Vbus.fValid = true;
            }
        else if (key == "Boot")
            {
            std::uint32_t nonce;
            std::cin >> nonce;
            m.Boot.v = (std::uint8_t) nonce;
            m.Boot.fValid = true;
            }
        else if (key == "Channel1")
            {
            std::cin >> m.TouchData.v.ch1;
            m.TouchData.fValid = true;
            }
        else if (key == "Channel2")
            {
            std::cin >> m.TouchData.v.ch2;
            m.TouchData.fValid = true;
            }
        else if (key == "Amplitude")
            {
            std::cin >> m.TouchData.v.amplitude;
            m.TouchData.fValid = true;
            }
        else if (key == "LeftTouchCounter")
            {
            std::cin >> m.TouchCount.v.touchCountLeft;
            m.TouchCount.fValid = true;
            }
        else if (key == "RightTouchCounter")
            {
            std::cin >> m.TouchCount.v.touchCountRight;
            m.TouchCount.fValid = true;
            }
        else if (key == ".")
            {
            putTestVector(m);
            m = m0;
            fAny = false;
            fUpdate = false;
            }
        else if (key == "")
            /* ignore empty keys */
            fUpdate = false;
        else
            {
            std::cerr << "unknown key: " << key << "\n";
            fUpdate = false;
            }

        fAny |= fUpdate;
        }

    if (!std::cin.eof() && std::cin.fail())
        {
        std::string nextword;

        std::cin.clear(std::cin.goodbit);
        std::cin >> nextword;
        std::cerr << "parse error: " << nextword << "\n";
        return 1;
        }

    if (fAny)
        putTestVector(m);

    return 0;
    }