const API_BASE_URL = 'https://flash-lookup-production.up.railway.app/api';

async function checkServerStatus() {
    try {
        const response = await fetch(`${API_BASE_URL}/search?word=test`);
        return response.ok;
    } catch (error) {
        return false;
    }
}

function showLoading() {
    document.getElementById("resultsSection").innerHTML = `
    <div class="loading">
      <div class="status-icon">⏳</div>
      Processing your request...
    </div>`;
}

function showError(message) {
    document.getElementById("resultsSection").innerHTML = `
    <div class="not-found">
      <span class="status-icon error">⚠️</span>
      ${message}
    </div>`;
}

function showServerError() {
    document.getElementById("resultsSection").innerHTML = `
    <div class="not-found">
      <span class="status-icon error">🔌</span>
      <strong>Backend Server Unavailable</strong><br>
      The dictionary service is temporarily unavailable. Please try again later.
    </div>`;
}

async function searchWord() {
    const word = document.getElementById("searchInput").value.trim();
    if (!word) {
        showError("Please enter a word to search");
        return;
    }

    showLoading();

    try {
        const response = await fetch(`${API_BASE_URL}/search?word=${encodeURIComponent(word)}`);

        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const result = await response.json();
        const el = document.getElementById("resultsSection");

        if (result.found) {
            const formattedDefinition = result.definition.replace(/\n/g, '<br>');

            el.innerHTML = `
        <div class="results-title"><span class="status-icon success">✅</span>Word Found!</div>
        <div class="word-result">
          <div class="word-title">${word.toLowerCase()}</div>
          <div class="word-definition">${formattedDefinition}</div>
        </div>`;
        } else {
            const corrections = result.corrections || [];
            const correctionHTML = corrections.length
                ? `<div style="margin-top:20px;"><strong>Did you mean:</strong>
           <ul class="suggestions-list">${corrections
                    .map(c => `<li onclick="searchSpecificWord('${c}')">${c}</li>`)
                    .join("")}</ul></div>`
                : "";

            el.innerHTML = `
        <div class="results-title"><span class="status-icon error">❌</span>Word Not Found</div>
        <div class="not-found">The word "<strong>${word}</strong>" was not found in the dictionary.${correctionHTML}</div>`;
        }
    } catch (error) {
        console.error('Search error:', error);
        showServerError();
    }
}

async function getAutoSuggestions() {
    const prefix = document.getElementById("searchInput").value.trim();
    if (!prefix) {
        showError("Please enter a prefix for suggestions");
        return;
    }

    showLoading();

    try {
        const response = await fetch(`${API_BASE_URL}/suggestions?prefix=${encodeURIComponent(prefix)}`);

        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const suggestions = await response.json();
        const el = document.getElementById("resultsSection");

        if (suggestions.length) {
            el.innerHTML = `
        <div class="results-title"><span class="status-icon info">💡</span>Auto-suggestions for "${prefix}"</div>
        <ul class="suggestions-list">${suggestions
                    .map(s => `<li onclick="searchSpecificWord('${s}')">${s}</li>`)
                    .join("")}</ul>`;
        } else {
            el.innerHTML = `
        <div class="not-found"><span class="status-icon">🤷‍♂️</span>No suggestions found for "<strong>${prefix}</strong>"</div>`;
        }
    } catch (error) {
        console.error('Suggestions error:', error);
        showServerError();
    }
}

async function getSpellingCorrections() {
    const word = document.getElementById("searchInput").value.trim();
    if (!word) {
        showError("Please enter a word for spell checking");
        return;
    }

    showLoading();

    try {
        const response = await fetch(`${API_BASE_URL}/corrections?word=${encodeURIComponent(word)}`);

        if (!response.ok) {
            throw new Error(`HTTP ${response.status}`);
        }

        const corrections = await response.json();
        const el = document.getElementById("resultsSection");

        if (corrections.length) {
            el.innerHTML = `
        <div class="results-title"><span class="status-icon info">✏️</span>Spelling corrections for "${word}"</div>
        <ul class="suggestions-list">${corrections
                    .map(c => `<li onclick="searchSpecificWord('${c}')">${c}</li>`)
                    .join("")}</ul>`;
        } else {
            el.innerHTML = `
        <div class="not-found"><span class="status-icon">✅</span>No spelling corrections needed for "<strong>${word}</strong>"</div>`;
        }
    } catch (error) {
        console.error('Corrections error:', error);
        showServerError();
    }
}

function searchSpecificWord(word) {
    document.getElementById("searchInput").value = word;
    searchWord();
}

document.addEventListener("DOMContentLoaded", async () => {
    console.log("Dictionary app started");

    const serverRunning = await checkServerStatus();

    if (serverRunning) {
        document.getElementById("resultsSection").innerHTML = `
      <div class="results-title"><span class="status-icon success">✅</span>Connected to C++ Server!</div>
      <div class="not-found">Ready to search! Try typing a word above. 🔍</div>`;
    } else {
        showServerError();
    }
});

const inputEl = document.getElementById("searchInput");

inputEl.addEventListener("keypress", e => {
    if (e.key === "Enter") searchWord();
});

let debounceTimer;
inputEl.addEventListener("input", e => {
    clearTimeout(debounceTimer);
    const value = e.target.value.trim();
    if (value.length >= 2) {
        debounceTimer = setTimeout(() => {

        }, 500);
    }
});