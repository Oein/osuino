
class OSUNote
{
public:
    int col;
    int time;
    int type;
    int endtime;
};

class OSUNote_wihtoutcol
{
public:
    int time;
    int endtime;
};

enum OSUFileReadingState
{
    FormatVersion,
    AnyHitObjectORMETA,
    HitObjects,
    HitObjects_X,
    HitObjects_Y,
    HitObjects_Time,
    HitObjects_Type,
    HitObjects_HitSound,
    HitObjects_EndTime,
    HitObjects_HitSample,
    HitObjects_NEWLINE,
};

class OSUFile
{
private:
    OSUFileReadingState rstate;
    bool load_line(IStringType line)
    {
        // printf("Line: %s\n", line.c_str());
        if (rstate == OSUFileReadingState::FormatVersion)
        {
            // check starts with "osu file format"
            IStringType format = "osu file format v";
            for (int i = 0; i < format.length(); i++)
            {
                if (line[i] != format[i])
                {
                    printf("Format error\n");
                    return false;
                }
            }
            rstate = OSUFileReadingState::AnyHitObjectORMETA;
            // printf("Format version: %s\n", line.substr(format.length()).c_str());

            return true;
        }
        if (rstate == OSUFileReadingState::AnyHitObjectORMETA)
        {
            IStringType format1 = "Title:";
            IStringType format2 = "Version:";
            IStringType format3 = "[HitObjects]";
            IStringType format4 = "BeatmapSetID:";
            IStringType format5 = "OverallDifficulty:";
            IStringType format6 = "Artist:";

            bool match1 = true;
            for (int i = 0; i < format1.length(); i++)
            {
                if (line[i] != format1[i])
                {
                    match1 = false;
                    break;
                }
            }

            if (match1)
            {
                title = line.substr(format1.length());
                return true;
            }

            bool match2 = true;
            for (int i = 0; i < format2.length(); i++)
            {
                if (line[i] != format2[i])
                {
                    match2 = false;
                    break;
                }
            }
            if (match2)
            {
                diffName = line.substr(format2.length());
                // printf("Diff name: %s\n", diffName.c_str());
                return true;
            }

            bool match3 = true;
            for (int i = 0; i < format3.length(); i++)
            {
                if (line[i] != format3[i])
                {
                    match3 = false;
                    break;
                }
            }

            if (match3)
            {
                rstate = OSUFileReadingState::HitObjects;
                // printf("HitObjects parse started\n");
                return true;
            }

            bool match4 = true;
            for (int i = 0; i < format4.length(); i++)
            {
                if (line[i] != format4[i])
                {
                    match4 = false;
                    break;
                }
            }
            if (match4)
            {
                beatmapSetID = line.substr(format4.length());
                // printf("BeatmapSetID: %s\n", beatmapSetID.c_str());
                return true;
            }

            bool match5 = true;
            for (int i = 0; i < format5.length(); i++)
            {
                if (line[i] != format5[i])
                {
                    match5 = false;
                    break;
                }
            }
            if (match5)
            {
                OverallDifficulty = line.substr(format5.length());
                // printf("OverallDifficulty: %s\n", OverallDifficulty.c_str());
                return true;
            }

            bool match6 = true;
            for (int i = 0; i < format6.length(); i++)
            {
                if (line[i] != format6[i])
                {
                    match6 = false;
                    break;
                }
            }
            if (match6)
            {
                Artist = line.substr(format6.length());
                // printf("Artist: %s\n", Artist.c_str());
                return true;
            }

            return true;
        }

        if (rstate == OSUFileReadingState::HitObjects)
        {
            OSUNote note;
            int state = OSUFileReadingState::HitObjects_X;
            IStringType buffer = "";
            for (int i = 0; i < line.length(); i++)
            {
                char c = line[i];
                if (state == OSUFileReadingState::HitObjects_X)
                {
                    if (c == ',')
                    {
                        int colData = string2int(buffer);
                        note.col = (int)(colData * 4 / 512);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_Y;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Y)
                {
                    if (c == ',')
                    {
                        state = OSUFileReadingState::HitObjects_Time;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Time)
                {
                    if (c == ',')
                    {
                        note.time = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_Type;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_Type)
                {
                    if (c == ',')
                    {
                        note.type = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_HitSound;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_HitSound)
                {
                    if (c == ',')
                    {
                        state = OSUFileReadingState::HitObjects_EndTime;
                    }
                    continue;
                }
                if (state == OSUFileReadingState::HitObjects_EndTime)
                {
                    if (c == ':')
                    {
                        note.endtime = string2int(buffer);
                        buffer = "";
                        state = OSUFileReadingState::HitObjects_NEWLINE;
                    }
                    else
                    {
                        buffer += c;
                    }
                    continue;
                }
            }

            if (state == OSUFileReadingState::HitObjects_X)
            {
                printf("End of hit objects\n");
                state = OSUFileReadingState::AnyHitObjectORMETA;
            }
            else if (state != OSUFileReadingState::HitObjects_NEWLINE)
            {
                printf("Wrong note data\n");
                return false;
            }
            else
            {
                if(note.endtime && note.time > note.endtime) {
                    int oldTime = note.time;
                    note.time = note.endtime;
                    note.endtime = oldTime;
                }
                notes[note.col].push_back({
                    note.time,
                    note.endtime,
                });
            }

            return true;
        }

        return true;
    }

public:
    IVector<OSUNote_wihtoutcol> notes[4];
    IStringType title;
    IStringType diffName;
    IStringType beatmapSetID;
    IStringType OverallDifficulty;
    IStringType Artist;

    // used for arduino
    IStringType filePath;

    bool load(IStringType fpath, bool loadOnlyMetadata = false)
    {
        filePath = fpath;
        return loadFrom(loadOnlyMetadata);
    }

    void unload()
    {
        notes[0].clear();
        notes[1].clear();
        notes[2].clear();
        notes[3].clear();
    }

    bool loadFrom(bool loadOnlyMetadata = false)
    {
        IStringType notedata = fsapi.get(filePath);

        DeltaTime startTime;
        startTime.deltaTime();

        notes[0].clear();
        notes[1].clear();
        notes[2].clear();
        notes[3].clear();

        rstate = OSUFileReadingState::FormatVersion;

        IStringType buffer = "";
        for (int i = 0; i < notedata.length(); i++)
        {
            char c = notedata[i];
            if (c == '\n')
            {
                bool res = load_line(buffer);
                if (!res)
                    return false;
                buffer = "";
            }
            else
            {
                buffer += c;
            }

            if (loadOnlyMetadata && rstate == OSUFileReadingState::HitObjects)
                break;
        }
        if (!(loadOnlyMetadata && rstate == OSUFileReadingState::HitObjects))
        {
            bool res = load_line(buffer);
            if (!res)
                return false;
        }

        // printf("Loaded notes of %lu %lu %lu %lu\n", notes[0].size(), notes[1].size(), notes[2].size(), notes[3].size());
        // printf("Loaded in %d ms\n", startTime.deltaTime());

        return true;
    }
};
