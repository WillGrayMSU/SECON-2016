$(document).ready(function () {
  'use strict';

  function updateSection(selection, id) {
    if (!$(selection).hasClass('active')) {
      $(selection).siblings().attr('class', '');
      $(selection).attr('class', 'active');
      $('.content-primary > section').removeClass().addClass('slide-out');
      setTimeout(function () {
        $('.content-primary > section').hide();
        $(id).show();
        $(id).removeClass().addClass('slide-in');
      }, 250);
    }
  }

  $('nav ul li').eq(0).click(function () {
    updateSection(this, '#home');
  });

  $('nav ul li').eq(1).click(function () {
    updateSection(this, '#team');
  });

  $('nav ul li').eq(2).click(function () {
    updateSection(this, '#deliverables');
  });

  $('nav ul li').eq(3).click(function () {
    updateSection(this, '#media');
  });

  $('nav ul li').eq(4).click(function () {
    updateSection(this, '#design');
  });

  Galleria.loadTheme("/scripts/galleria/themes/galleria-classicmod/galleria.classicmod.js");
  Galleria.run(".galleria");

});
