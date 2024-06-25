import type { CapacitorConfig } from '@capacitor/cli';

const config: CapacitorConfig = {
  appId: 'digital.guima.laringoscopiot',
  appName: 'LaringoscópIOT',
  webDir: 'dist',
  server: {
    cleartext: true, // allow http traffic (not https)
    androidScheme: 'http',
  }
};

export default config;
