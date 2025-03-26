import { BeatmapDecoder } from "osu-parsers";
import { $ } from "bun";

const number2base64 = (num: number): string => {
  const base =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  const result = [];
  while (num > 0) {
    result.push(base[num % 64]);
    num = Math.floor(num / 64);
  }
  return result.join("");
};

const arrayIntoString = (arr: string[]): string => {
  const count = arr.length;
  const countStr = number2base64(count).padStart(2, "_");
  return (
    countStr +
    arr.map((v) => number2base64(v.length).padStart(2, "_") + v).join("")
  );
};

const stringIntoParseableString = (str: string): string => {
  return number2base64(str.length).padStart(2, "_") + str;
};

async function work(filepath: string) {
  const content = await await Bun.file(filepath).text();
  const parsed = new BeatmapDecoder().decodeFromString(content);
  const minus = parsed.general.audioLeadIn;
  const title = parsed.metadata.title;
  const artist = parsed.metadata.artist;
  const version = parsed.metadata.version;
  const mapid = parsed.metadata.beatmapId;
  const setid = parsed.metadata.beatmapSetId;

  const hitObjects: string[][] = [[], [], [], []];
  let objCounts: number[] = [0, 0, 0, 0];
  parsed.hitObjects.forEach((v) => {
    const x = Math.floor((v.startX / 512) * 4);
    let startTime = v.startTime;
    // @ts-ignore
    let endTime = v.endTime || 0;

    startTime -= minus;
    endTime -= minus;

    hitObjects[x].push(
      [
        number2base64(startTime).padStart(4, "_"),
        endTime == 0 ? "@" : number2base64(endTime).padStart(4, "_"),
      ].join("")
    );
  });

  objCounts = hitObjects.map((v) => v.length);

  // split each array into 100 items
  const hitObjectsStr = hitObjects.map((v, column) => {
    const result: string[] = [];
    for (let i = 0; i < v.length; i += 100) {
      const joined = v.slice(i, i + 100).join("");
      result.push(number2base64(joined.length).padStart(3, "_") + joined);
      console.log(
        "Got chunk size",
        result[result.length - 1].length,
        "for column",
        column
      );
    }
    return result;
  });
  const columnsChunkCounts = hitObjectsStr.map((v) =>
    number2base64(v.length).padStart(2, "_")
  );
  const columnNoteCounts = objCounts
    .map((v) => number2base64(v).padStart(3, "_"))
    .join("");

  const filenameHash = require("crypto")
    .createHash("md5")
    .update(filepath)
    .digest("hex");
  const _5digitHash = parseInt(filenameHash.substring(0, 5), 16) + 10000000;

  let netresp =
    number2base64(setid).padStart(6, "_") +
    number2base64(_5digitHash).padStart(6, "_") +
    stringIntoParseableString(title) +
    stringIntoParseableString(artist) +
    stringIntoParseableString(version) +
    columnsChunkCounts.join("") +
    columnNoteCounts +
    hitObjectsStr.map((x) => x.join("")).join("");

  // MARK: - Convert

  const mapFile = netresp;

  function base64ToNumber(base64: string) {
    const base =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    let num = 0;
    let power = 1;

    for (let i = 0; i < base64.length; i++) {
      if (base64[i] === "_") {
        continue;
      }
      let index = base.indexOf(base64[i]);
      if (index !== -1) {
        num += index * power;
        power *= 64;
      } else {
        console.error(
          `Invalid base64 character: ${base64[i]}, parsing ${base64}`
        );
        return -1; // Return an error code for invalid input
      }
    }

    return num;
  }

  function base64ToLargeNumber(base64: string) {
    const base =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    let num = 0;
    let power = 1;

    for (let i = 0; i < base64.length; i++) {
      if (base64[i] === "_") {
        continue;
      }
      let index = base.indexOf(base64[i]);
      if (index !== -1) {
        num += index * power;
        power *= 64;
      } else {
        console.error(
          `Invalid base64 character: ${base64[i]}, parsing ${base64}`
        );
        return -1; // Return an error code for invalid input
      }
    }

    return num;
  }

  class OSUINOFile {
    cursor: number;
    constructor() {
      this.cursor = 0;
    }

    parseString(length: number) {
      // get length characters from cursor
      let str = mapFile.substring(this.cursor, this.cursor + length);
      this.cursor += length;
      // parse that str into number
      let num = base64ToNumber(str);
      // read num characters from cursor
      let result = mapFile.substring(this.cursor, this.cursor + num);
      this.cursor += num;
      return result;
    }

    getStringLength(length: number) {
      let str = mapFile.substring(this.cursor, this.cursor + length);
      this.cursor += length;
      return str;
    }

    parse() {
      let res: { [key: string]: string } = {};
      console.log("Parsing file");
      this.cursor = 0;

      let setid = base64ToLargeNumber(this.getStringLength(6));
      let id = base64ToLargeNumber(this.getStringLength(6));
      let title = this.parseString(2);
      let artist = this.parseString(2);
      let version = this.parseString(2);

      console.log(`Title: ${title}`);
      console.log(`Artist: ${artist}`);
      console.log(`Version: ${version}`);

      let chunksSizes = [
        base64ToNumber(this.getStringLength(2)),
        base64ToNumber(this.getStringLength(2)),
        base64ToNumber(this.getStringLength(2)),
        base64ToNumber(this.getStringLength(2)),
      ];

      let noteCounts = [
        base64ToLargeNumber(this.getStringLength(3)),
        base64ToLargeNumber(this.getStringLength(3)),
        base64ToLargeNumber(this.getStringLength(3)),
        base64ToLargeNumber(this.getStringLength(3)),
      ];

      let metaDataLength =
        6 +
        6 +
        title.length +
        artist.length +
        version.length +
        2 + // title length header
        2 + // artist length header
        2 + // version length header
        8 + // chunks sizes
        12; // note counts
      console.log(`Metadata length: ${metaDataLength}`);
      res[`map-${id}.meta`] = mapFile.substring(0, metaDataLength);

      for (let i = 0; i < 4; ++i) {
        for (let j = 0; j < chunksSizes[i]; j++) {
          let chunk = this.parseString(3);
          res[`chunk-${id}-${i}-${j}.chunk`] = chunk;
        }
      }

      return res;
    }
  }

  const osuinoFile = new OSUINOFile();
  let r = osuinoFile.parse();

  for (let key in r) {
    if (key.startsWith("chunk")) {
      await Bun.write(`osuino/chunks/${key}`, r[key]);
    } else if (key.startsWith("map")) {
      await Bun.write(`osuino/meta/${key}`, r[key]);
    }
  }

  const audioFile = parsed.general.audioFilename;
  const audioPath = path.join(__dirname, filepath, "..", audioFile);
  const wavPath = path.join(__dirname, "osuino/audio", `${_5digitHash}.wav`);
  // convert mp3 to wav for arduino
  // 16000Hz, 8bits per sample, mono

  await $`ffmpeg -y -i "${audioPath}" -acodec pcm_u8 -ar 16000 -ac 1 "${wavPath}"`;
}

import fs from "fs";
import path from "path";

// work through the file
const queue = ["exports"];
while (queue.length > 0) {
  let current = queue.shift();
  if (!current) break;
  if (current.endsWith(".osu")) {
    await work(current);
    continue;
  }
  if (current.endsWith(".mp3") || current.startsWith(".")) continue;
  const files = fs.readdirSync(current);
  for (let file of files) {
    queue.push(`${current}/${file}`);
  }
}
