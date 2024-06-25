# it's a good idea to pin this, but for demo purposes we'll leave it as is
FROM node:20 as builder

# automatically creates the dir and sets it as the current working dir
WORKDIR /usr/src/app
# this will allow us to run vite and other tools directly
ENV PATH /usr/src/node_modules/.bin:$PATH

COPY package.json ./
COPY yarn.lock ./

RUN yarn install

# use a more specific COPY, as this will include files like `Dockerfile`, we don't really need inside our containers.
COPY . ./

FROM builder as dev
CMD ["yarn", "dev"]

FROM builder as prod-builder
RUN yarn build

# it's a good idea to pin this, but for demo purposes we'll leave it as is
FROM nginx:latest as prod

# Create a directory for the Nginx configuration files
RUN mkdir /etc/nginx/templates

# Add nginx configuration to the default conf file
RUN echo 'server { \
    listen 80; \
    server_name localhost; \
    location / { \
        root /usr/share/nginx/html; \
        index index.html; \
        try_files $uri $uri/ /index.html; \
        add_header Access-Control-Allow-Origin "*"; \
        add_header Access-Control-Allow-Methods "GET, POST, PUT, DELETE, OPTIONS"; \
        add_header Access-Control-Allow-Headers "DNT,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Range"; \
        add_header Access-Control-Expose-Headers "Content-Length,Content-Range"; \
        if ($request_method = OPTIONS) { \
            return 204; \
        } \
    } \
}' > /etc/nginx/conf.d/default.conf

# Copy build output to serve
COPY --from=prod-builder /usr/src/app/dist /usr/share/nginx/html

CMD ["nginx", "-g", "daemon off;"]