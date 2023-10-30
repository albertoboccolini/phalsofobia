FROM gcc:latest

COPY / /phalsofobia
WORKDIR /phalsofobia

RUN gcc -o phalsofobia main.c gamelib.c -std=c11 -Wall

CMD [ "./phalsofobia" ]