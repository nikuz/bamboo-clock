const hotColors = [
    'FF0000',
    '0000FF',
    '00FF00',
    'FFFF00',
    '0FFFFF',
    'FF00FF',
    'FFFFFF',
    'rainbow',
];

const makeHotColorActive = (id) => {
    const selectedCl = 'selected'
    const cells = document.querySelectorAll('.color-cell');
    const colorPicker = document.getElementById('color-picker');

    cells.forEach(item => {
        if (item.id === id) {
            item.classList.add(selectedCl);
        } else {
            item.classList.remove(selectedCl);
        }
    });

    if (colorPicker.id === id) {
        colorPicker.classList.add(selectedCl);
    } else {
        colorPicker.classList.remove(selectedCl);
    }
};

const showError = () => {
    const errorTemplate = document.getElementById('error-template');
    const error = errorTemplate.content.cloneNode(true);
    document.body.appendChild(error);

    const background = document.getElementById('error-bg');
    background.addEventListener('click', hideError, false);

    const closeButton = document.getElementById('error-button');
    closeButton.addEventListener('click', hideError, false);
};

const hideError = () => {
    document.body.removeChild(document.getElementById('error'));
}

const request = (url) => {
    const loadingEl = document.getElementById('loading');
    loadingEl.style.display = 'block';

    return window.fetch(url).then((response) => {
        loadingEl.style.display = 'none';
        if (response.status !== 200) {
            setTimeout(showError, 200);
        }
        return response;
    }).catch(() => {
        setTimeout(showError, 200);
        loadingEl.style.display = 'none';
    });
};

const watchTimePicker = (event) => {
    request(`/time?value=${event.target.value}`);
};

const watchNtpSwitch = (event) => {
    const timePicker = document.getElementById('time-picker');
    const wifiForm = document.getElementById('wifi-form');
    const visible = event.target.checked;
    wifiForm.style.display = visible ? 'block' : 'none';
    timePicker.disabled = visible;

    request(`/ntp?value=${visible ? 1 : 0}`);
};

const watchBrightnessSwitch = (event) => {
    const brightnessRange = document.getElementById('brightness-range-container');
    const checked = event.target.checked;
    brightnessRange.style.display = checked ? 'none' : 'block';

    request(`/brightness?auto=${checked ? 1 : 0}`);
};

const watchBrightness = (event) => {
    request(`/brightness?value=${event.target.value}`);
};

const watchColorPicker = (event) => {
    const target = event.target;
    const hotColorsSelected = target instanceof HTMLLIElement;
    let color;

    if (target instanceof HTMLInputElement) {
        color = target.value.replace('#', '');
    } else if (hotColorsSelected) {
        color = target.getAttribute('data-color');
    }

    if (color) {
        request(`/color?value=${color}`).then((response) => {
            if (response.status === 200) {
                makeHotColorActive(target.id);
            }
        });
    }
};

const watchWifiSettings = (event) => {
    event.preventDefault();

    const form = event.target;
    const ssid = form['wifi-ssid'].value;
    const password = form['wifi-password'].value;

    if (ssid !== '') {
        request(`/wifi?ssid=${ssid}&password=${password}`);
    }
};

const saveHandler = () => {
    request(`/save`).then((response) => {
        if (response.status === 200) {
            window.close();
        }
    });
};

document.addEventListener('DOMContentLoaded', () => {
    const timePicker = document.getElementById('time-picker');
    timePicker.addEventListener('blur', watchTimePicker, false);

    const ntpSwitch = document.getElementById('ntp-switch');
    ntpSwitch.addEventListener('change', watchNtpSwitch, false);

    const colorsList = document.getElementById('colors-list');
    colorsList.addEventListener('click', watchColorPicker, false);

    const brightnessSwitch = document.getElementById('auto-brightness-switch');
    brightnessSwitch.addEventListener('change', watchBrightnessSwitch, false);

    const brightnessRange = document.getElementById('brightness-range');
    brightnessRange.addEventListener('change', watchBrightness, false);

    const save = document.getElementById('save');
    save.addEventListener('click', saveHandler, false);

    hotColors.forEach(item => {
        const element = document.createElement('li');
        let className = 'color-cell';
        if (item === 'rainbow') {
            className += ` ${item}`;
        } else {
            element.style.backgroundColor = `#${item}`;
        }
        element.className = className;
        element.id = `color-${item}`;
        element.setAttribute('data-color', item);

        colorsList.appendChild(element);
    });

    const colorPicker = document.getElementById('color-picker');
    colorPicker.addEventListener('change', watchColorPicker, false);

    const wifiForm = document.getElementById('wifi-form');
    wifiForm.addEventListener('submit', watchWifiSettings, false);

    request(`/sync`).then((response) => {
        if (response.status === 200) {
            response.text().then((text) => {
                try {
                    const data = JSON.parse(text);

                    timePicker.value = data.time;
                    if (data.ntp === 1) {
                        ntpSwitch.checked = true;
                        wifiForm.style.display = 'block';
                        timePicker.disabled = true;
                    }
                    document.getElementById('wifi-ssid').value = data.ssid;

                    brightnessSwitch.checked = data.brightnessAuto === 1;
                    brightnessRange.value = data.brightness;
                    if (data.brightnessAuto === 0) {
                        document.getElementById('brightness-range-container').style.display = 'block';
                    }

                    if (hotColors.includes(data.color)) {
                        makeHotColorActive(`color-${data.color}`);
                    } else if (data.color !== '') {
                        colorPicker.value = `#${data.color}`;
                        colorPicker.classList.add('selected');
                    }
                } catch (e) {}
            });
        }
    });
});