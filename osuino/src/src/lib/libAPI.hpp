#include "../env.hpp"

#ifndef _API_MOCK_
#include <WiFiClientSecure.h>
#include <UrlEncode.h>

const char *ssid = ENV_WIFI_SSID;     // your network SSID (name of wifi network)
const char *password = ENV_WIFI_PASS; // your network password

const char *server = ENV_SERVER_HOST; // Server URL

const char *test_root_ca =
    R"(
-----BEGIN CERTIFICATE-----
MIICCTCCAY6gAwIBAgINAgPlwGjvYxqccpBQUjAKBggqhkjOPQQDAzBHMQswCQYD
VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEUMBIG
A1UEAxMLR1RTIFJvb3QgUjQwHhcNMTYwNjIyMDAwMDAwWhcNMzYwNjIyMDAwMDAw
WjBHMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2Vz
IExMQzEUMBIGA1UEAxMLR1RTIFJvb3QgUjQwdjAQBgcqhkjOPQIBBgUrgQQAIgNi
AATzdHOnaItgrkO4NcWBMHtLSZ37wWHO5t5GvWvVYRg1rkDdc/eJkTBa6zzuhXyi
QHY7qca4R9gq55KRanPpsXI5nymfopjTX15YhmUPoYRlBtHci8nHc8iMai/lxKvR
HYqjQjBAMA4GA1UdDwEB/wQEAwIBhjAPBgNVHRMBAf8EBTADAQH/MB0GA1UdDgQW
BBSATNbrdP9JNqPV2Py1PsVq8JQdjDAKBggqhkjOPQQDAwNpADBmAjEA6ED/g94D
9J+uHXqnLrmvT/aDHQ4thQEd0dlq7A/Cr8deVl5c1RxYIigL9zC2L7F8AjEA8GE8
p/SgguMh1YQdc4acLa/KNJvxn7kjNuK8YAOdgLOaVsjh4rsUecrNIdSUtUlD
-----END CERTIFICATE-----
)";

WiFiClientSecure client;
#endif

template <typename T>
class NetworkResponse
{
public:
    bool success;
    T data;

    NetworkResponse(bool s)
    {
        success = s;
    }
    NetworkResponse(bool s, T d)
    {
        success = s;
        data = d;
    }
};

#ifdef _API_MOCK_
class MockStreamParser
{
private:
    IStringType content;
    int cursor = 0;
    int base64ToNumber(const IStringType base64)
    {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int num = 0;
        int power = 1;

        for (int i = 0; i < base64.length(); i++)
        {
            if (base64[i] == '_')
            {
                continue;
            }
            int index = -1;
            for (int j = 0; j < base.length(); ++j)
            {
                if (base[j] == base64[i])
                {
                    index = j;
                    break;
                }
            }
            if (index != -1)
            {
                num += index * power;
                power *= 64;
            }
            else
            {
                printf("Invalid base64 character(mock int): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }

        return num;
    }

public:
    MockStreamParser(IStringType content)
    {
        this->content = content;
    }

    unsigned long base64ToLargeNumber(const IStringType base64)
    {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        unsigned long num = 0;
        unsigned long power = 1;

        for (int i = 0; i < base64.length(); i++)
        {
            if (base64[i] == '_')
            {
                continue;
            }
            int index = -1;
            for (int j = 0; j < base.length(); ++j)
            {
                if (base[j] == base64[i])
                {
                    index = j;
                    break;
                }
            }
            if (index != -1)
            {
                num += index * power;
                power *= 64;
            }
            else
            {
                printf("Invalid base64 character(mock long): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }

        return num;
    }

    void consumeHeader()
    {
    }

    IStringType getStringOfLength(int length)
    {
        IStringType ret = subString(this->content, cursor, length);
        cursor += length;
        return ret;
    }

    int getNumber(int length)
    {
        return base64ToNumber(getStringOfLength(length));
    }

    unsigned long getLargeNumber(int length)
    {
        return base64ToLargeNumber(getStringOfLength(length));
    }

    IStringType parseString(int length)
    {
        int len = getNumber(length);
        return getStringOfLength(len);
    }
};
#else
#ifndef _IS_ARDUINO_
#error "NetworkStreamParser is only available for Arduino"
#endif
class NetworkStreamParser
{
private:
    int base64ToNumber(const IStringType base64)
    {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        int num = 0;
        int power = 1;

        for (int i = 0; i < base64.length(); i++)
        {
            if (base64[i] == '_')
            {
                continue;
            }
            int index = -1;
            for (int j = 0; j < base.length(); ++j)
            {
                if (base[j] == base64[i])
                {
                    index = j;
                    break;
                }
            }
            if (index != -1)
            {
                num += index * power;
                power *= 64;
            }
            else
            {
                printf("Invalid base64 character(api int): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }

        return num;
    }

    unsigned long base64ToLargeNumber(const IStringType base64)
    {
        const IStringType base = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        unsigned long num = 0;
        unsigned long power = 1;

        for (int i = 0; i < base64.length(); i++)
        {
            if (base64[i] == '_')
            {
                continue;
            }
            int index = -1;
            for (int j = 0; j < base.length(); ++j)
            {
                if (base[j] == base64[i])
                {
                    index = j;
                    break;
                }
            }
            if (index != -1)
            {
                num += index * power;
                power *= 64;
            }
            else
            {
                printf("Invalid base64 character(api long): %c, parsing %s\n", base64[i], base64.c_str());
                return -1; // Return an error code for invalid input
            }
        }

        return num;
    }

public:
    void consumeHeader()
    {
        while (client.connected())
        {
            IStringType line = client.readStringUntil('\n');
            if (line == "\r")
            {
                printf("headers received\n");
                break;
            }
        }
    }

    IStringType getStringOfLength(int length)
    {
        IStringType result = "";
        // printf("==(%d)> ", length);
        while (length > 0)
        {
            if (client.available())
            {
                char c = client.read();
                result += c;
                length--;
                // printf("%c", c);
            }
        }
        // printf("\n");
        return result;
    }

    int getNumber(int length)
    {
        return base64ToNumber(getStringOfLength(length));
    }

    unsigned long getLargeNumber(int length)
    {
        return base64ToLargeNumber(getStringOfLength(length));
    }

    IStringType parseString(int length)
    {
        int len = getNumber(length);
        return getStringOfLength(len);
    }
};
#endif

class OSUBeatmapItem
{
public:
    unsigned long id;
    IStringType version;
    float star;
};

class OSUMapResponseItem
{
public:
    IStringType title;
    IStringType artist;
    std::vector<OSUBeatmapItem> maps;
};

class OSUBeatResponseItem
{
public:
    IStringType title;
    IStringType artist;
    OSUBeatmapItem map;

    OSUBeatResponseItem(IStringType title, IStringType artist, OSUBeatmapItem map)
    {
        this->title = title;
        this->artist = artist;
        this->map = map;
    }
};

#define IOSUMapResponse NetworkResponse<std::vector<OSUMapResponseItem>>

class OSUAPI
{
public:
    IOSUMapResponse search(IStringType keyword = "", int page = 0)
    {
#ifdef _API_MOCK_
        MockStreamParser parser("_UEC_TAeven naer tid gaar_RBilly Qvarnstroem_D_c_H4826580_M[7K] Lunatic_D6.4_b_H5000417_K[7K] Extra_E7.48_d_H5000418_M[7K] Extreme_E9.479B_cI'm not a HERO (2023 Remake)_HWyvernP_C_m_H4888373_V[4K] He's a Real HERO_E4.35_q_H4970739_Z[4K] [Lunar] Sticks' Hard_E3.42kB_ZGrabinschrift der Goetter_Zikaruga_nex vs. Kagetora._B_q_H4965556_Z[7K] R.I.P::rest_in_peace_E9.36IB_SMy Name is Malguem_UMinyeongi Deryeookke_B_a_H5000408_J[7K] Hard_E2.95_+_IRagnarok_HUnknown_B_n_H5000395_Z[4K] Earth's Final Sunset_B0_6_Qyou are my curse_Hnyamura_B_b_H5000394_N[4K] memories_B0nD_UIt's Boss Time, dude_HAethral_F_h_H4974244_Q[4K] Boss [DONE]_E6.15_q_H5000383_Z[4K] Challenge [NOT DONE]_E6.15_l_H5000384_U[4K] Easy [NOT DONE]_E6.15_l_H5000385_U[4K] Hard [NOT DONE]_E6.15_n_H5000386_W[4K] Normal [NOT DONE]_E6.15GC_FFansa_dPastel_Palettes x Minato Aqua_D_a_H4979961_J[7K] LOVE_E4.04_c_H4980409_L[7K] Normal_E2.37_i_H4980410_R[7K] oid45's Hard_E3.339B_SSeimeisei Syndrome_bCamellia feat. Hatsune Miku_C_c_H5000358_L[7K] APPEND_E6.28_q_H5000359_Z[4K] SYNDROME SYNDROME!!!_E6.28_+_MOde To THR3A_FZeppy_B_l_H4988877_U[10K] Plastic Guitar_E6.21_0_GMy War_KLil Fityma_B_c_H5000353_L[4K] freaky_E6.62MG_hDe Em Luong Thien (Phara - Vol 6)_IAn Clock_J_m_H5000343_W[4K] mine [1.05x Rate]_D5.9_n_H5000344_W[4K] mine [1.15x Rate]_E6.36_m_H5000345_V[4K] mine [1.1x Rate]_E6.14_n_H5000346_W[4K] mine [1.25x Rate]_E6.84_l_H5000347_V[4K] mine [1.2x Rate]_D6.6_m_H5000348_W[4K] mine [1.35x Rate]_D7.3_m_H5000349_V[4K] mine [1.3x Rate]_E7.07_m_H5000350_V[4K] mine [1.4x Rate]_E7.55_a_H5000351_J[4K] mine_E5.67WD_WThe (un)cliched ending_Mpolysha & CK_D_4_H4837719_n[4K] Huh...is it truly the end? // Hard_E3.57EB_H4837720_z[4K] Surely this is not the end... right? // Insane_E4.56_s_H4857335_b[4K] Wait...what? // Normal_E2.31VB_MLost Requiem_HLudicin_B_6_H4997867_p[4K] The Final Symphony of a Fading Realm_E7.66VB_oMesmerizer (DJ Raisei's Maximizer Remix)_JDJ Raisei_B_c_H5000331_L[4K] Insane_E6.25XB_UAether Crest: Astral_hvoid (Mournfinale) x Kenji Mizuno_B_a_H5000329_M[4K] Eternal_B0cH_GRocket_fTsukisaka Sayu (CV: Kito Akari)_J_x_H5000304_g[4K] Stardust 0.95x (166bpm) OD8_E5.25_t_H5000305_f[4K] Stardust 0.9x (157bpm) OD8_B5_x_H5000306_g[4K] Stardust 1.05x (184bpm) OD8_E5.74_x_H5000307_g[4K] Stardust 1.15x (201bpm) OD8_E6.24_w_H5000308_f[4K] Stardust 1.1x (192bpm) OD8_E5.99_x_H5000309_g[4K] Stardust 1.25x (219bpm) OD8_E6.73_w_H5000310_f[4K] Stardust 1.2x (210bpm) OD8_E6.48_w_H5000311_f[4K] Stardust 1.3x (227bpm) OD8_E6.98_e_H5000312_N[4K] Stardust_E5.49_2_FTitle_GArtist_B_j_H5000327_P[5K] Difficulty_H2170.26jB_3za kazdym razem (kyoto Frenchcore Remix) (Sped Up Ver.)_EJula_B_g_H5000325_P[7K] mapping ai_E6.798C_WQuevedo (Sped Up Ver.)_OBZRP & QUEVEDO_F_b_H5000177_K[4K] [1.1]_E4.61_b_H5000317_K[4K] [1.2]_E4.96_b_H5000318_K[4K] [1.3]_E5.31_b_H5000319_K[4K] [1.4]_E5.66_c_H5000320_L[4K] hanabi_E4.25");
#else
        NetworkStreamParser parser;
        if (!client.connect(server, 443))
            return IOSUMapResponse(false);
        client.print("GET https://");
        client.print(ENV_SERVER_HOST);
        client.print("/s?t=");
        client.print(ENV_API_TOKEN);
        if (keyword != "")
        {
            client.print("&q=");
            client.print(urlEncode(keyword.c_str()).c_str());
        }
        client.print("&p=");
        client.print(String(page).c_str());
        client.println(" HTTP/1.0");
        client.println("Host: osu-workers.oein0219.workers.dev");
        client.println("Connection: close");
        client.println();
        parser.consumeHeader();
#endif

        unsigned long length = parser.getLargeNumber(2);
        std::vector<OSUMapResponseItem> result;
        for (int i = 0; i < length; i++)
        {
            OSUMapResponseItem item;
            // printf("Getting map %d out of %zu\n", i, length);
            parser.getStringOfLength(2); // consume length
            item.title = parser.parseString(2);
            // printf("  Title: %s\n", item.title.c_str());
            item.artist = parser.parseString(2);
            // printf("  Artist: %s\n", item.artist.c_str());
            unsigned long mapLength = parser.getLargeNumber(2);
            for (int j = 0; j < mapLength; j++)
            {
                // printf("  Getting beat %d out of %zu\n", j, mapLength);
                OSUBeatmapItem map;
                parser.getStringOfLength(2); // consume length
                map.id = string2int(parser.parseString(2));
                // printf("    ID: %lu\n", map.id);
                map.version = parser.parseString(2);
                // printf("    Version: %s\n", map.version.c_str());
                map.star = string2float(parser.parseString(2));
                // printf("    Star: %f\n", map.star);
                item.maps.push_back(map);
            }
            result.push_back(item);
        }

#ifndef _API_MOCK_
        client.stop();
#endif
        return IOSUMapResponse(true, result);
    }
};