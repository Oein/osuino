/**
 * Welcome to Cloudflare Workers! This is your first worker.
 *
 * - Run `npm run dev` in your terminal to start a development server
 * - Open a browser tab at http://localhost:8787/ to see your worker in action
 * - Run `npm run deploy` to publish your worker
 *
 * Bind resources to your worker in `wrangler.jsonc`. After adding bindings, a type definition for the
 * `Env` object can be regenerated with `npm run cf-typegen`.
 *
 * Learn more at https://developers.cloudflare.com/workers/
 */

import { BeatmapDecoder } from "osu-parsers";

interface Covers {
	cover: string;
	"cover@2x": string;
	card: string;
	"card@2x": string;
	list: string;
	"list@2x": string;
	slimcover: string;
	"slimcover@2x": string;
}
interface Hype {
	current: number;
	required: number;
}
interface Requiredmeta {
	main_ruleset: number;
	non_main_ruleset: number;
}
interface Nominationssummary {
	current: number;
	eligible_main_rulesets?: string[];
	required_meta: Requiredmeta;
}
interface Availability {
	download_disabled: boolean;
	more_information?: any;
}
interface Failtimes {
	exit: number[];
	fail: number[];
}
interface Owner {
	id: number;
	username: string;
}
interface Beatmap {
	beatmapset_id: number;
	difficulty_rating: number;
	id: number;
	mode: string;
	status: string;
	total_length: number;
	user_id: number;
	version: string;
	accuracy: number;
	ar: number;
	bpm: number;
	convert: boolean;
	count_circles: number;
	count_sliders: number;
	count_spinners: number;
	cs: number;
	deleted_at?: any;
	drain: number;
	hit_length: number;
	is_scoreable: boolean;
	last_updated: number;
	mode_int: number;
	passcount: number;
	playcount: number;
	ranked: number;
	url: string;
	checksum?: string | string;
	failtimes: Failtimes;
	max_combo?: null | number | number;
	owners: Owner[];
	top_tag_ids: any[];
	last_checked: number;
}
interface Convert {
	beatmapset_id: number;
	difficulty_rating: number;
	id: number;
	mode: string;
	status: string;
	total_length: number;
	user_id: number;
	version: string;
	accuracy: number;
	ar: number;
	bpm: number;
	convert: boolean;
	count_circles: number;
	count_sliders: number;
	count_spinners: number;
	cs: number;
	deleted_at?: any;
	drain: number;
	hit_length: number;
	is_scoreable: boolean;
	last_updated: string;
	mode_int: number;
	passcount: number;
	playcount: number;
	ranked: number;
	url: string;
	checksum?: any;
	failtimes: Failtimes;
	owners: Owner[];
}
interface Description {
	description: string;
}
interface Genre {
	id: number;
	name: string;
}
interface Relateduser {
	avatar_url: string;
	country_code: string;
	default_group: string;
	id: number;
	is_active: boolean;
	is_bot: boolean;
	is_deleted: boolean;
	is_online: boolean;
	is_supporter: boolean;
	last_visit?: null | string | string;
	pm_friends_only: boolean;
	profile_colour?: (null | string)[];
	username: string;
}
interface User {
	avatar_url: string;
	country_code: string;
	default_group: string;
	id: number;
	is_active: boolean;
	is_bot: boolean;
	is_deleted: boolean;
	is_online: boolean;
	is_supporter: boolean;
	last_visit?: string;
	pm_friends_only: boolean;
	profile_colour?: any;
	username: string;
}
interface SearchResult {
	id: number;
	artist: string;
	artist_unicode: string;
	creator: string;
	source: string;
	tags: string;
	title: string;
	title_unicode: string;
	next_update: number;
	covers: Covers;
	favourite_count: number;
	hype: Hype;
	nsfw: boolean;
	offset: number;
	play_count: number;
	preview_url: string;
	spotlight: boolean;
	status: string;
	track_id?: number;
	user_id: number;
	video: boolean;
	bpm: number;
	can_be_hyped: boolean;
	deleted_at?: any;
	discussion_enabled: boolean;
	discussion_locked: boolean;
	is_scoreable: boolean;
	last_updated: number;
	legacy_thread_url?: string;
	nominations_summary: Nominationssummary;
	ranked: number;
	ranked_date?: any;
	storyboard: boolean;
	submitted_date: number;
	availability: Availability;
	has_favourited: boolean;
	beatmaps: Beatmap[];
	converts: Convert[];
	current_nominations: any[];
	description: Description;
	genre: Genre;
	language: Genre;
	pack_tags: any[];
	ratings: number[];
	recent_favourites: any[];
	related_users: Relateduser[];
	user: User;
	last_checked: number;
	rating: number;
	related_tags: any[];
}

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

export default {
	async fetch(request, env, ctx): Promise<Response> {
		const url = new URL(request.url);
		if (url.searchParams.get("t") !== env.TOKEN) {
			return new Response("Unauthorized", { status: 401 });
		}
		const path = url.pathname;
		if (path == "/s") {
			// implement search
			const keyword = url.searchParams.get("q") || "";
			const page = url.searchParams.get("p") || "0";
			const pageInt = parseInt(page);
			if (isNaN(pageInt) || pageInt < 0) {
				return new Response("@", { status: 400 });
			}
			const fch = (await (
				await fetch(
					`https://sg.catboy.best/api/v2/search?q=${keyword}&limit=20&offset=${
						20 * pageInt
					}&mode=3&keys=4`,
					{
						headers: {
							"User-Agent": "osuino!workers (v1.0.0)",
						},
					}
				)
			).json()) as SearchResult[];
			// need title, artist, map difficultyList, map ids
			const result = fch.map((v) => {
				const title = stringIntoParseableString(v.title);
				const artist = stringIntoParseableString(v.artist);
				const maps = arrayIntoString(
					v.beatmaps.map(
						(v) =>
							stringIntoParseableString(v.id.toString()) +
							stringIntoParseableString(v.version) +
							stringIntoParseableString(v.difficulty_rating.toString())
					)
				);
				return title + artist + maps;
			});
			return new Response(arrayIntoString(result));
		}
		if (path == "/d") {
			// implement home page
			const id = url.searchParams.get("i");
			if (!id) {
				return new Response("@", { status: 400 });
			}
			// ex: 4996697
			const fch = await (
				await fetch(`https://sg.catboy.best/osu/${id}`, {
					headers: {
						"User-Agent": "osuino!workers (v1.0.0)",
					},
				})
			).text();
			const parsed = new BeatmapDecoder().decodeFromString(fch);
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
						endTime == 0
							? "@"
							: number2base64(endTime).padStart(4, "_"),
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
					console.log("Got chunk size",result[result.length - 1].length,"for column",column);
				}
				return result;
			});
			const columnsChunkCounts = hitObjectsStr.map((v) =>
				number2base64(v.length).padStart(2, "_")
			);
			const columnNoteCounts = objCounts.map((v) =>
				number2base64(v).padStart(3, "_")
			).join("");

			return new Response(
				number2base64(setid).padStart(6, "_") +
				number2base64(mapid).padStart(6, "_") +
				stringIntoParseableString(title) +
					stringIntoParseableString(artist) +
					stringIntoParseableString(version) +
					columnsChunkCounts.join("") +
					columnNoteCounts +
					hitObjectsStr.map(x => x.join("")).join(""),
			);
		}
		return new Response("Hello from osuino!");
	},
} satisfies ExportedHandler<Env>;
