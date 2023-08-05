import { createApp } from "vue";
import { createI18n, type I18nOptions } from "vue-i18n";
import * as en from "../locales/en.json";
import * as ptBr from "../locales/pt-BR.json";
import "./style.css";
import App from "./App.vue";

const options: I18nOptions = {
  legacy: false,
  locale: "en",
  fallbackLocale: "en",
  messages: {
    en,
    "pt-BR": ptBr,
  },
};

const i18n = createI18n<false, typeof options>(options);
const app = createApp(App);

app.use(i18n);
app.mount("#app");
