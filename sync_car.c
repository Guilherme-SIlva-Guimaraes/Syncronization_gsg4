#include <pthread.h>
#include "sync_car.h"

void station_init(struct station *station)
{
	pthread_mutex_init(&station->lock, NULL);
	pthread_cond_init(&station->car_arrived, NULL);
	pthread_cond_init(&station->all_boarded, NULL);
	station->waiting = 0;
	station->to_board = 0;
	station->boarding = 0;
}

void station_load_car(struct station *station, int count)
{
	pthread_mutex_lock(&station->lock);
	if (count > 0 && station->waiting > 0) {
		station->to_board = station->waiting < count ? station->waiting : count;
		pthread_cond_broadcast(&station->car_arrived);
		while (station->to_board > 0 || station->boarding > 0)
			pthread_cond_wait(&station->all_boarded, &station->lock);
	}
	pthread_mutex_unlock(&station->lock);
}

void station_wait_for_car(struct station *station)
{
	pthread_mutex_lock(&station->lock);
	station->waiting++;
	while (station->to_board == 0)
		pthread_cond_wait(&station->car_arrived, &station->lock);
	station->to_board--;
	station->waiting--;
	station->boarding++;
	pthread_mutex_unlock(&station->lock);
}

void station_on_board(struct station *station)
{
	pthread_mutex_lock(&station->lock);
	station->boarding--;
	if (station->to_board == 0 && station->boarding == 0)
		pthread_cond_signal(&station->all_boarded);
	pthread_mutex_unlock(&station->lock);
}
