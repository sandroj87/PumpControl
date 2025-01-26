// PumpControl.cpp
#include "PumpControl.h"

// Costruttore
PumpControl::PumpControl(uint8_t pin, bool logic, float flow) {
    _pin = pin;
    _logic = logic;
    _stato = false;
    _durataFine = 0;
    _timerAttivo = false;
    _flow = flow;
    pinMode(_pin,OUTPUT);
    off();
}

// Taratura flusso pompa
void PumpControl::setFlow(float flow){
    _flow = flow;
}

// Imposta i tempi per il gocciolamento
void PumpControl::setDropTimes(unsigned long dropTimeOn, unsigned long dropTimeOff){
    _dropTimeOn  = dropTimeOn;
    _dropTimeOff = dropTimeOff;
}

// Resetta il contatore per il numero totale di gocce
void PumpControl::resetDropCounter(){
    _totalDrops = 0;
}

// Restituisce true se lo stato della pompa è cambiato dall'ultimo aggiornamento
bool PumpControl::stateChanged() {
    if( _lastState != _stato){
        _lastState = _stato;
        return true;
    }else{
        return false;
    }
}

// Restituisce lo stato attuale della pompa
bool PumpControl::getState(){
    return _stato;
}

// Accendi la pompa
void PumpControl::on() {
    _stato = true;
    digitalWrite(_pin, _logic ? HIGH : LOW);
    _timerAttivo = false; // Disabilita eventuali timer
}

// Accendi la pompa per una durata temporale specifica
void PumpControl::onFor(unsigned long durataMs) {
    on();
    _durataFine  = millis() + durataMs;
    _timerAttivo = true;
}

// Spegni la pompa
void PumpControl::off() {
    _stato = false;
    digitalWrite(_pin, _logic ? LOW : HIGH);
    _timerAttivo = false; // Disabilita eventuali timer
}

// Froza lo spegnimento disattivando anche eventuale gocciolamento
void PumpControl::stop() {
    _drop = false;
    off();
}

// Eroga un certo volume (flusso continuo)
void PumpControl::addVolume(float volume){
    _volume = volume;
    if (_flow <= 0.0f) {
        // Errore: flusso non impostato o zero
        return;
    }
    unsigned long durataMs = (unsigned long)((_volume / _flow) * 1000); // Converti in millisecondi
    onFor(durataMs);
}

// Eroga un certo numero di gocce
void PumpControl::drop(unsigned long nDrops){
    _nDrops = nDrops;
    _drop   = true;
    _dropOn = true;
    _timeNext = millis() + _dropTimeOn;
    _dropCount  = 1;
    _totalDrops = 1;
    on();
}

// Restituisce il numero di gocce erogate
unsigned long PumpControl::getDrops(){
    return _totalDrops;
}

// Aggiornamento dello stato della pompa (da chiamare regolarmente nel loop)
void PumpControl::update() {

    unsigned long now = millis();
    
    // Gestione normale del timer
    if (_timerAttivo && now >= _durataFine) {
        off();
    }

    // Gestione dei cicli per le gocce
    if(_drop && now >= _timeNext){
        if(_dropOn){
            // Passa alla fase off
            off();
            _dropOn = false;
            _timeNext = now + _dropTimeOff;
        }else{
            if(_dropCount < _nDrops){
                on();
                _dropCount++;
                _totalDrops++;
                _dropOn = true;
                _timeNext = now + _dropTimeOn;
            }else{
                _drop = false;
            }
        }
    }
}