from __future__ import absolute_import, print_function

from tweepy.streaming import StreamListener
from tweepy import OAuthHandler
from tweepy import Stream

import paho.mqtt.publish

"""
Para que nuestra app funcione necesitamos registranos como desarrolladores en
http://dev.twitter.com/apps. Una vez creemos nuestra cuenta y una aplicación
tendremos los datos para rellenar.
"""
consumer_key=""
consumer_secret=""
access_token=""
access_token_secret=""

""" Podemos obtener la ID de un usuario en http://gettwitterid.com """
user_id=""

"""
Creamos un listener. Este objeto espera los nuevos tuits y hace algo con ellos,
por ejemplo, mostrarlos por pantalla.
"""
class StdOutListener(StreamListener):

    """
    Este método se encarga de imprimir los nuevos tuits
    """
    def on_status(self, status):
        print(status.text)
        return True

    """
    Este método se encarga de imprimir el mensaje que recibimos cuando obtenemos
    un error al recibir el tuit
    """
    def on_error(self, status):
        print(status)

class MQTTListener(StreamListener):

    """
    Este método se encarga de imprimir los nuevos tuits y enviarlo al broker
    MQTT
    """
    def on_status(self, status):
        publish.single("esibot/seminario6/", status.text, hostname="localhost")
        return True

"""
Esta es la función principal de nuestro programa aquí vamos a crear un listener,
vamos a hacer que éste inicie sesión con las credenciales anteriores y lo vamos
a poner a la escucha de tuits
"""
if __name__ == '__main__':
    """ Creamos el listener """
    l = StdOutListener()

    """ Iniciamos sesión """
    auth = OAuthHandler(consumer_key, consumer_secret)
    auth.set_access_token(access_token, access_token_secret)

    """ Creamos un stream dónde recibiremos tuits """
    stream = Stream(auth, l)

    """ Iniciamos el stream para que escuche a un usuario """
    stream.filter(follow=[user_id])