import chalk from "chalk";

const origlog = console.log;

class Logger {
  constructor() {}
  timeStr() {
    const now = new Date();
    return `${now.getHours().toString().padStart(2, "0")}:${now
      .getMinutes()
      .toString()
      .padStart(2, "0")}:${now.getSeconds().toString().padStart(2, "0")}`;
  }
  colorTime() {
    return `[${this.timeStr()}]`;
  }
  log(...args: any[]) {
    origlog(this.colorTime(), chalk.gray("(default) "), ...args);
  }
  info(...args: any[]) {
    origlog(this.colorTime(), chalk.blue("(   info) "), ...args);
  }
  success(...args: any[]) {
    origlog(this.colorTime(), chalk.green("(success) "), ...args);
  }
  warn(...args: any[]) {
    origlog(this.colorTime(), chalk.yellow("(   warn) "), ...args);
  }
  error(...args: any[]) {
    origlog(this.colorTime(), chalk.red("(  error) "), ...args);
  }
  query(...args: any[]) {
    origlog(this.colorTime(), chalk.gray("(  query) "), ...args);
  }
  loading(...args: any[]) {
    origlog(this.colorTime(), chalk.gray("(loading) "), ...args);
  }
}

const logger = new Logger();

export default logger;
