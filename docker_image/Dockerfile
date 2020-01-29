FROM ubuntu:19.10
LABEL maintainer="Philip Hönnecke <p.hoennecke@tu-braunschweig.de>"
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
    && apt-get install -y \
      git make g++ libsfml-dev
RUN apt-get install -y \
      zip tar