package itmo.se.lab3

import itmo.se.lab3.helpers.RunWithChrome
import itmo.se.lab3.helpers.UserCredentials
import itmo.se.lab3.helpers.WebDriverTemplateInvocationContextProvider
import itmo.se.lab3.pages.HomePage
import itmo.se.lab3.pages.MarketplaceCartPage
import itmo.se.lab3.pages.MarketplacePage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class MarketplaceTest {
    @TestTemplate
    @RunWithChrome
    fun `it allows the user to filter articles by keywords`(driver: WebDriver) {
        val marketPage = MarketplacePage(driver)
        marketPage.keywordsField.sendKeys("radeon")
        marketPage.searchButton.click()
        marketPage.waitForItems()

        val articles = marketPage.listItems()
        assertNotEquals(0, articles.size)
        assertEquals("Оптимальный выбор видеокарт при сборке фермы майнинга", articles[0].title)
        assertTrue(articles[0].getKeywords().contains("Radeon"))
    }

    @TestTemplate
    @RunWithChrome
    fun `it allows the user to sort articles by price`(driver: WebDriver) {
        val marketPage = MarketplacePage(driver)

        val topItemPriceWithoutSorting = marketPage.listItems().first().price
        assertTrue(topItemPriceWithoutSorting < 1000)

        marketPage.sortByPriceToggle.click()
        marketPage.waitForItems()

        val topItemPriceWithSorting = marketPage.listItems().first().price
        assertTrue(topItemPriceWithSorting > 1000)
    }

    @TestTemplate
    @RunWithChrome
    fun `it allows the user to add articles to cart`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val emptyCartPage = MarketplaceCartPage(driver)
        emptyCartPage.listItems().forEach { it.removeFromCart() }

        val marketPage = MarketplacePage(driver)
        marketPage.keywordsField.sendKeys("radeon")
        marketPage.searchButton.click()
        marketPage.waitForItems()

        val articles = marketPage.listItems()
        assertEquals(554.72, articles[0].price)
        assertEquals("Оптимальный выбор видеокарт при сборке фермы майнинга", articles[0].title)
        articles[0].addToCart()

        val cartPage = MarketplaceCartPage(driver)
        val items = cartPage.listItems()
        assertEquals(1, items.size)
        assertEquals("Оптимальный выбор видеокарт при сборке фермы майнинга", items[0].title)

        val cartTotal = cartPage.getTotalIncludingFee()
        assertEquals(605.15, cartTotal)
    }
}